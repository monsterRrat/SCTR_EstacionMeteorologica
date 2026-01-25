#include <stdio.h>
#include <time.h>
#include <math.h>
#include <DHT20.h>
#include <sgp30.h>

DHT20 sensor1

struct datos {
	float temp;
	float hum;
	uint16_t co2;
};



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
    uint16_t tvoc;
    sgp30_init(&sgp30);
    sgp30_iaq_init(&sgp30);

	print_temp();
	enum state st = TEMP;

	for (;;) {

        sgp30_read(&sgp30, p_datos->co2, &tvoc);
        getSensor1
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