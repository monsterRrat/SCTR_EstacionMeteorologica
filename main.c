#include <stdio.h>
#include <time.h>
#include <math.h>
#include <DHT20.h>
#include <sgp30.h>

typedef struct
{
    uint8_t inited;

    uint8_t (*iic_init)(void);
    uint8_t (*iic_deinit)(void);
    uint8_t (*iic_read_cmd)(uint8_t addr, uint8_t *buf, uint16_t len);
    uint8_t (*iic_write_cmd)(uint8_t addr, uint8_t *buf, uint16_t len);

    void (*delay_ms)(uint32_t ms);
    void (*debug_print)(const char *fmt, ...);

} sgp30_handle_t;
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
}

DHT20 sensor1

struct datos {
	float temp;
	float hum;
	uint16_t co2;
};

uint16_t calculate_absolute_humidity(float temperature, float humidity)
{
    float Es, E, AH;
    
    Es = 6.112f * expf((17.62f * temperature) / (243.12f + temperature));
    E  = (humidity / 100.0f) * Es;
    AH = (216.7f * E) / (273.15f + temperature);

    return (uint16_t)(AH * 256.0f);
}

void getSensor1(p_datos, sensor) {
    updateMeasurement(&sensor);

    float t = getTemperature(&sensor);
    float h = getHumidity(&sensor);
    p_datos->temp = t
    p_datos->hum = h
};

int tiempoUnix() {
    int time_t segundos = time(NULL);
    return segundos;
};


enum state {
	TEMP = 0,
	HUMEDAD,
    CO2,
	STATE_MAX
};

enum event {
	NONE = 0,
	TIEMPO,
	EVENT_MAX
};


void print_temp(void)
{
	printf("LED: ON\n");
}

void print_hum(void)
{
	printf("LED: OFF\n");
}

void print_co2(void)
{
	printf("LED: OFF\n");
}


enum state trans_temp(void)
{
	print_temp();
	return TEMP;
}

enum state trans_humedad(void)
{
	print_hum();
	return HUMEDAD;
}

enum state trans_co2(void)
{
	print_hum();
	return CO2;
}

enum state (*trans_table[STATE_MAX][EVENT_MAX])(void) = {
	[TEMP] = {
		[TIEMPO] = trans_humedad
	},
	[HUM] = {
		[TIEMPO] = trans_co2
	},
    [CO2] = {
		[TIEMPO] = trans_temp
	},
};

enum event event_parser(int ch)
{
	if (ch%5 == 0)
		return TIEMPO;

	return NONE;
}

int main(void)
{
	DHT20_init()
    sgp30_handle_t sgp30;
    sgp30.iic_init      = sgp30_iic_init;
    sgp30.iic_deinit    = NULL;              
    sgp30.iic_read_cmd  = sgp30_iic_read_cmd;
    sgp30.iic_write_cmd = sgp30_iic_write_cmd;
    sgp30.delay_ms      = sgp30_delay_ms;
    sgp30.debug_print   = NULL;
    sgp30_init(&sgp30);
    sgp30_iaq_init(&sgp30);
    
	print_temp();
	enum state st = TEMP;

	for (;;) {

        sgp30_read(&sgp30, p_datos->co2, &tvoc);
        getSensor1
        uint16_t ah = calculate_absolute_humidity(t, h);
        sgp30_set_absolute_humidity(&sgp30, ah);
        sgp30_read(&sgp30, p_datos->co2, &tvoc);

		int ch = tiempoUnix();
		enum event ev = event_parser(ch);
		enum state (*tr)(void) = trans_table[st][ev];

		if (tr == NULL) { 
			printf("Transicion no definida (st=%d, ev=%d)\n", st, ev);
			continue;
		}
		st = tr(); 
	}

	return 0;
}