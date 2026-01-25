#include <stdio.h>
#include <time.h>

int tiempoUnix() {
	/*Obtener el instante de tiempo actual*/
    time_t segundos = time(NULL);
	/*Imprimir la marca de tiempo actual*/
    printf("Tiempo Unix actual: %ld\n", (long)segundos);

    return segundos;
}

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

/* Acciones */
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

/* Manejador de transición (transition handler)
   Funciones asociadas a las transiciones*/
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
/* Tabla de transición: apunta a handlers concretos */
enum state (*trans_table[STATE_MAX][EVENT_MAX])(void) = {
	[TEMP] = {
		[TIEMPO] = trans_humedad  /*Despues de la cte. de tiempo, pasas de temp a hum*/
	},
	[HUM] = {
		[TIEMPO] = trans_co2     /*Despues de la cte. de tiempo, pasas de hum a CO2*/
	},
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
	/*Estado inicial*/
	printf("LED: 0=apaga, 1=enciende\n");
	print_temp();
	enum state st = TEMP;

	for (;;) {

		/* Espera comentarios de teclado */
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