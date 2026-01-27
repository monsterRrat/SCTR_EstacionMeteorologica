/* Include standard headers */
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
/* Include Pico SDK headers */
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
/* Include driver headers */
#include "DHT20/DHT20.h"
#include <sgp30/driver_sgp30.h>
#include <ssd1306/ssd1306.h>

typedef struct {
	float temp;
	float hum;
	uint16_t co2;
} datos;

uint8_t sgp30_iic_init(void);

uint8_t sgp30_iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len);

uint8_t sgp30_iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len);

void sgp30_delay_ms(uint32_t ms);

uint16_t calculate_absolute_humidity(float temperature, float humidity);

void getSensor1(datos *p_datos, DHT20 sensor);

time_t tiempoUnix(void) { return time(NULL); }

static void oled_print_value(ssd1306_t* o, const char* label, const char* value);

/*Estados del sistema*/
enum state {
	TEMP = 0,
	HUMEDAD,
    CO2,
	STATE_MAX
};

/*Eventos de entrada del sistema
  None representa una entrada irrelevante*/
enum event {
	NONE = 0,
	TIEMPO,
	EVENT_MAX
};

/* Acciones.
 * Funciones que dan pie a la lectura de sensores
 * Se desacoplan de los estados para tener máxima flexibilidad
 * en su uso.
 */

static void print_temp(float temp, ssd1306_t* o) {
    char buf[32];
    snprintf(buf, sizeof buf, "%.1f C", temp);
    oled_print_value(o, "TEMP", buf);
}
static void print_hum(float hum, ssd1306_t* o) {
    char buf[32];
    snprintf(buf, sizeof buf, "%.1f %%", hum);
    oled_print_value(o, "HUMEDAD", buf);
}
static void print_co2(uint16_t co2, ssd1306_t* o) {
    char buf[32];
    snprintf(buf, sizeof buf, "%u ppm", co2);
    oled_print_value(o, "CO2", buf);
}


/* Manejador de transición (transition handler)
   Funciones asociadas a las transiciones*/
enum state trans_temp(datos* p, ssd1306_t* o)
{
	print_temp(p->temp, o);
	return TEMP;
}

enum state trans_humedad(datos *p, ssd1306_t* o)
{
	print_hum(p->hum, o);
	return HUMEDAD;
}

enum state trans_co2(datos *p, ssd1306_t *o)
{
	print_co2(p->co2, o);
	return CO2;
}

<<<<<<< HEAD
/* Tabla de transición*/
enum state (*trans_table[STATE_MAX][EVENT_MAX])(datos *p_datos, ssd1306_t *oled) = {
    [TEMP] = {
        [TIEMPO] = trans_humedad  /*Despues de la cte. de tiempo, pasas de temp a hum*/
    },
    [HUMEDAD] = {
        [TIEMPO] = trans_co2     /*Despues de la cte. de tiempo, pasas de hum a CO2*/
    },
=======
enum state (*trans_table[STATE_MAX][EVENT_MAX])(datos *p_datos, oled) = {
	[TEMP] = {
		[TIEMPO] = trans_humedad  /*Despues de la cte. de tiempo, pasas de temp a hum*/
	},
	[HUMEDAD] = {
		[TIEMPO] = trans_co2     /*Despues de la cte. de tiempo, pasas de hum a CO2*/
	},
>>>>>>> d939f533c1619ceb78c1a15eb6cc0d70d7f22843
    [CO2] = {
        [TIEMPO] = trans_temp    /*Despues de la cte. de tiempo, pasas de CO2 a temp*/
    },
};

/* Parseador de eventos: mundo real → evento 
   Permite interactuar con la máquina de estados desde el exterior*/
enum event event_parser(int ch)
{
	if (ch%5 == 0)
		return TIEMPO;

	return NONE;
}

int main(void)
{
    struct datos datos_sensor;
    struct datos *p_datos = NULL;
    p_datos = &datos_sensor;
	
    i2c_init(i2c0, 400 * 1000); 
    gpio_set_function(4, GPIO_FUNC_I2C); 
    gpio_set_function(5, GPIO_FUNC_I2C); 
    gpio_pull_up(4);
    gpio_pull_up(5);

    ssd1306_t oled;
    if (!ssd1306_init(&oled, 128, 64, 0x3C, i2c0)) {
        printf("Error inicializando la pantalla!\n");
        return 1;
    }
    DHT20 sensor;
    int status = DHT20_init(&sensor);

    if (status == DHT20_OK) {
        printf("DHT20 inicializado correctamente.\n");
    } else {
        printf("Error al inicializar DHT20: %d\n", status);
        return 1;
    }
    sgp30_handle_t sgp30;
    sgp30.iic_init      = sgp30_iic_init;
    sgp30.iic_deinit    = NULL;              
    sgp30.iic_read_cmd  = sgp30_iic_read_cmd;
    sgp30.iic_write_cmd = sgp30_iic_write_cmd;
    sgp30.delay_ms      = sgp30_delay_ms;
    sgp30.debug_print   = NULL;
    sgp30_init(&sgp30);
    sgp30_iaq_init(&sgp30);
    
    enum state st = TEMP;
    uint16_t tvoc = 0;

    for (;;) {

        getSensor1(p_datos,sensor);
        uint16_t ah = calculate_absolute_humidity(p_datos->temp, p_datos->hum);
        sgp30_set_absolute_humidity(&sgp30, ah);
        sgp30_read(&sgp30, p_datos->co2, &tvoc);

		int ch = tiempoUnix();
		enum event ev = event_parser(ch);
		enum state (*tr)(datos*, oled) = trans_table[st][ev];

		if (tr == NULL) { 
			printf("Transicion no definida (st=%d, ev=%d)\n", st, ev);
			continue;
		}
		st = tr(); 
	}

	return 0;
}

uint8_t sgp30_iic_init(void)
{
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    return 0;
}

uint8_t sgp30_iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    if (i2c_write_blocking(i2c0, addr, buf, len, false) < 0)
        return 1;
    return 0;
}

uint8_t sgp30_iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    if (i2c_read_blocking(i2c0, addr, buf, len, false) < 0)
        return 1;
    return 0;
}

void sgp30_delay_ms(uint32_t ms)
{
    sleep_ms(ms);
};

uint16_t calculate_absolute_humidity(float temperature, float humidity)
{
    float Es, E, AH;
    
    Es = 6.112f * expf((17.62f * temperature) / (243.12f + temperature));
    E  = (humidity / 100.0f) * Es;
    AH = (216.7f * E) / (273.15f + temperature);

    return (uint16_t)(AH * 256.0f);
};

void getSensor1(datos *p_datos, DHT20 sensor) {
    updateMeasurement(&sensor);

    float t = getTemperature(&sensor);
    float h = getHumidity(&sensor);
    p_datos->temp = t;
    p_datos->hum = h;
}

static void oled_print_value(ssd1306_t* o, const char* label, const char* value) {
    ssd1306_clear(o);
    ssd1306_draw_string(o, 0, 0, 1, label);
    ssd1306_draw_string(o, 0, 16, 2, value);
    ssd1306_show(o);
}