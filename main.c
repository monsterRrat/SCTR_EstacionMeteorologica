/* Include standard headers */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
/* Include Pico SDK headers */
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
/* Drivers */
#define I2C_INST i2c0
#include "DHT20/DHT20.h"
#include "ssd1306.h"

/*Configuración del i2c para los perifericos*/
#define I2C_PORT       i2c0
#define I2C_SDA_PIN    4
#define I2C_SCL_PIN    5
#define I2C_BAUDRATE   (400 * 1000)

/* Direcciones típicas OLED */
#define OLED_ADDR      0x3C

/* Periodos sensor y FSM */
#define DHT20_SAMPLE_PERIOD_MS  1500u
#define FSM_PERIOD_MS           5000u

/* Estructura para almacenar datos
adquiridos del sensor DHT20 */
typedef struct {
    float temp;
    float hum;
} datos;

/*Enums estados y transiciones*/
enum state {
    TEMP = 0,
    HUMEDAD,
    STATE_MAX
};

enum event {
    NONE = 0,
    TIEMPO,
    EVENT_MAX
};

/*Event parser*/
static uint32_t last_slot = 0;

static enum event event_parser_ms(uint32_t ms_now)
{
    uint32_t slot = ms_now / FSM_PERIOD_MS;   // cambia cada 5 segundos
    if (slot != last_slot) {
        last_slot = slot;
        return TIEMPO;
    }
    return NONE;
}

/* Prototipos */
static uint8_t iic_init(void);

static void get_temp_hum(datos* p_datos, DHT20* sensor);

static void oled_show_temp(ssd1306_t* oled, float t);
static void oled_show_hum(ssd1306_t* oled, float h);

/*Acciones*/
static void print_temp(float temp) {
    printf("TEMP: %.2f C\n", temp);
}
static void print_hum(float hum) {
    printf("HUMEDAD: %.2f %%\n", hum);
}

/* Transiciones */
static enum state trans_temp(datos* p)
{
    print_temp(p->temp);
    return TEMP;
}

static enum state trans_humedad(datos* p)
{
    print_hum(p->hum);
    return HUMEDAD;
}

/* Tabla de transición */
static enum state(*trans_table[STATE_MAX][EVENT_MAX])(datos* p_datos) = {
    [TEMP] = {
        [TIEMPO] = trans_humedad
    },
    [HUMEDAD] = {
        [TIEMPO] = trans_temp
    },
};

/*MAIN*/
int main(void)
{
    stdio_init_all();
    sleep_ms(1200);

    datos datos_sensores = { 0 };
    datos* p_datos = &datos_sensores;
    enum state st = TEMP;

    /* I2C init */
    iic_init();

    /* OLED init (driver nuevo) */
    ssd1306_t oled = { 0 };
    oled.external_vcc = false;  // típico en módulos 0.96" con charge-pump interno
    if (!ssd1306_init(&oled, 128, 64, OLED_ADDR, I2C_PORT)) {
        printf("ERROR: no se pudo inicializar OLED (malloc o I2C)\n");
        while (true) sleep_ms(1000);
    }

    /* Mensaje de arranque */
    ssd1306_clear(&oled);
    ssd1306_draw_string(&oled, 0, 0, 1, "SSD1306 OK");
    ssd1306_draw_string(&oled, 0, 16, 1, "DHT20 + FSM");
    ssd1306_show(&oled);
    sleep_ms(1200);

    /* DHT20 init */
    DHT20 sens_temp_hum;
    int st_dht = DHT20_init(&sens_temp_hum);
    if (st_dht != DHT20_OK) {
        printf("Error al inicializar DHT20: %d\n", st_dht);
        while (true) sleep_ms(1000);
    }
    printf("DHT20 inicializado correctamente.\n");

    /* Forzar “slot” inicial estable */
    uint32_t now_ms = to_ms_since_boot(get_absolute_time());
    last_slot = now_ms / FSM_PERIOD_MS;

    /* Timer de muestreo DHT20 */
    uint32_t next_dht_ms = now_ms;

    /* Render inicial */
    oled_show_temp(&oled, p_datos->temp);

    for (;;) {
        now_ms = to_ms_since_boot(get_absolute_time());

        /* 1) Muestreo DHT20 cada 1.5 s */
        if ((int32_t)(now_ms - next_dht_ms) >= 0) {
            get_temp_hum(p_datos, &sens_temp_hum);
            next_dht_ms = now_ms + DHT20_SAMPLE_PERIOD_MS;
        }

        /* 2) FSM por tiempo (5 s) */
        enum event ev = event_parser_ms(now_ms);
        enum state(*tr)(datos * p) = trans_table[st][ev];

        if (tr != NULL) {
            st = tr(p_datos);

            /* 3) OLED sincronizada con el cambio cada 5 s */
            if (st == TEMP) {
                oled_show_temp(&oled, p_datos->temp);
            }
            else {
                oled_show_hum(&oled, p_datos->hum);
            }
        }

        sleep_ms(20);
    }

    // opcional si algún día sales del bucle
    // ssd1306_deinit(&oled);
    // return 0;
}

/*Funcion de inicialización del I2C*/
static uint8_t iic_init(void)
{
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // pull-ups internos (mejor externos 4.7k)
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    return 0;
}

/* Funcion para adquisicon de datos DHT20 */
static void get_temp_hum(datos* p_datos, DHT20* sensor)
{
    int st = getMeasurement(sensor);

    if (st == DHT20_OK) {
        p_datos->temp = getTemperature(sensor);
        p_datos->hum = getHumidity(sensor);
    }
    else if (st == DHT20_ERROR_LASTREAD) {
        return;
    }
    else {
        printf("DHT20 read error: %d\n", st);
    }
}

/*Funciones de visualizacion en OLED*/
static void oled_show_temp(ssd1306_t* oled, float t)
{
    char line[24];
    snprintf(line, sizeof(line), "T: %.1fºC", t);

    ssd1306_clear(oled);
    ssd1306_draw_string(oled, 0, 0, 1, "TEMPERATURA");
    ssd1306_draw_string(oled, 0, 16, 2, line);  // scale 2 para que destaque
    ssd1306_show(oled);
}

static void oled_show_hum(ssd1306_t* oled, float h)
{
    char line[24];
    snprintf(line, sizeof(line), "HUM: %.1f %%", h);

    ssd1306_clear(oled);
    ssd1306_draw_string(oled, 0, 0, 1, "HUMEDAD");
    ssd1306_draw_string(oled, 0, 16, 2, line);
    ssd1306_show(oled);
}
