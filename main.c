#include <stdio.h>
#include <time.h>


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
    printf("LED: 0=apaga, 1=enciende\n");
	print_temp();
	enum state st = TEMP;

	for (;;) {

	
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