#include <chipKITEthernet.h>
#include "funciones.h"

#define MESSAGE_SIZE 	8192
#define ARRAY_SIZE		MESSAGE_SIZE / 2
// Enter a MAC address and IP address for your controller below. 
// A zero MAC address means that the chipKIT MAC is to be used
byte mac[] = {	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

//			 !!MODIFY THIS!!
// The IP address will be dependent on your local network:
byte ip[] = { 
	10,0,0,8 };

byte gateway[] = { 10,0,0,1 };
byte subnet[] = { 255, 255, 255, 0 };

// telnet defaults to port 23
Server server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet

typedef enum comandos {
	NOP,
	HELP,
	EXIT,
	BUBBLE0,
	BUBBLE1,
	BUBBLE2,
	BUBBLE3,
	QUICK0,
	QUICK1,
	QUICK2,
	QUICK3,
	SEL0,
	SEL1,
	SEL2,
	SEL3,
} comandos;

void print_array(int *array, int len);
int copia_a_resultado(int size);
comandos deco_comando(void);
void ejecuta_comando(Client *client, comandos, int);

void setup() 
{
	// initialize the ethernet device
	//	Ethernet.begin(mac, ip, gateway, subnet);
	Ethernet.begin(mac,ip);
	// start listening for clients
	server.begin();
	// open the serial port
	Serial.begin(9600);
}

uint8_t message[MESSAGE_SIZE];
char comando[16];
int number_array[ARRAY_SIZE];
int resultado[ARRAY_SIZE];


void loop() 
{
	// wait for a new client:
	Client client = server.available();
	uint message_size = 0;
	int cantidad_num = 0;
	int n = 0;
	int size = 0;
	int data_ini = 0;
	comandos num_comando = NOP;

	if (client) {
		message = {0};
		comando = {0};
		number_array = {NULL};

		message_size = client.available();
		if(message_size)
		{
			//message_size = client.read(message, 1000);
			int m = 0;
			char c; 
			do {
				c = client.read();
				if(c != -1) {
					message[m] = c;
					if(m < MESSAGE_SIZE - 1) {
						m++;
					} else {
						client.println("Array demasiado largo");
						message = {0};
						m = 0;
						break;
					}
				}
			} while(c != '\n');
			

			//Serial.println((char *) message);

			//message_size = strlen((char *) message);
			/*if (m > 0 && m < MESSAGE_SIZE - 1)
			{
				message[m] = '\n';
				message_size = m + 1;
			}
			*/
			message_size = m;
			Serial.print("Message size: ");
			Serial.println(message_size);

			for (int i = 0; i < message_size; ++i)
			{
				//message[i] = client.read();
				if(message[i] == ' ' || message[i] == '\r' || message[i] == '\n')
				{
					if(cantidad_num == 0)
					{
						if(i>16){
							num_comando = NOP;
							cantidad_num = 0;
							break;
						}
						memcpy(comando, message, i);
						if((num_comando = deco_comando()) != NOP)
						{
							data_ini = i + 1;
						}
					}
					cantidad_num++;
				}
			}

			Serial.println(comando);

			if(cantidad_num > 0) //Tenemos datos
			{ 
				char numero[16] = {0};
				int j = 0;
				n = 0;
				for(int i = data_ini; i < message_size; ++i)
				{
					if(message[i] != ' ' && message[i] != '\n')
					{
						numero[j] = message[i];
						j++;
					} else {
						//number_array[n] = atoi(numero);
						if(sscanf(numero, "%d", &number_array[n]) < 1)
						{
							Serial.println("Error al convertir");
						Serial.print("Num: ");
							Serial.println(numero);
							size = 0;
							number_array = {NULL};
							break;
						} else
						{
						 n++;
						 j = 0;
						 numero = {0};
						}
					}
				}
				size = n;
				Serial.print("Tamaño del array: ");
				Serial.println(size);
			}
		}
		
		client.flush();

		ejecuta_comando(&client, num_comando, size);
		if(size)
		{
			print_array(resultado, size);
		}
	}
}

void print_array(int *array, int len)
{
	for(int i = 0;i < len; ++i)
	{
		server.print(array[i]);
		Serial.print(array[i]);
		server.print(' ');
		Serial.print(' ');
	}
	server.print('\n');
	Serial.println("");
}

int copia_a_resultado(int size)
{
	
	for (int i = 0; i < size; ++i)
	{
		resultado[i] = number_array[i];
	}
	return 0;
}

comandos deco_comando(void)
{
	if (!strcmp(comando, "bubble0"))
	{
		return BUBBLE0;
	} else if (!strcmp(comando, "bubble1"))
	{
		return BUBBLE1;
	} else if (!strcmp(comando, "bubble2"))
	{
		return BUBBLE2;
	} else if (!strcmp(comando, "bubble3"))
	{
		return BUBBLE3;
	} else if (!strcmp(comando, "quick0"))
	{
		return QUICK0;
	} else if (!strcmp(comando, "quick1"))
	{
		return QUICK1;
	} else if (!strcmp(comando, "quick2"))
	{
		return QUICK2;
	} else if (!strcmp(comando, "quick3"))
	{
		return QUICK3;
	} else if (!strcmp(comando, "sel0"))
	{
		return SEL0;
	} else if (!strcmp(comando, "sel1"))
	{
		return SEL1;
	} else if (!strcmp(comando, "sel2"))
	{
		return SEL2;
	} else if (!strcmp(comando, "sel3"))
	{
		return SEL3;
	} else if (!strcmp(comando, "help"))
	{
		return HELP;
	} else if (!strcmp(comando, "exit"))
	{
		return EXIT;
	} else {
		return NOP;
	}
}

void ejecuta_comando(Client *client, comandos comando, int size)
{
	uint16_t antes_micros, despues_micros, antes_millis, despues_millis;
	copia_a_resultado(size);
	switch(comando)
	{
		case BUBBLE0:
			antes_micros = micros();
			antes_millis = millis();
			bubble0(resultado, size);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo Bubble0 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Bubble0 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case BUBBLE1:
			antes_micros = micros();
			antes_millis = millis();
			bubble1(resultado, size);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo Bubble1 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Bubble1 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case BUBBLE2:
			antes_micros = micros();
			antes_millis = millis();
			bubble2(resultado, size);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo Bubble2 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Bubble2 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case BUBBLE3:
			antes_micros = micros();
			antes_millis = millis();
			bubble3(resultado, size);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo Bubble3 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Bubble3 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case QUICK0:
			antes_micros = micros();
			antes_millis = millis();
			quicksort0(resultado, 0, size - 1);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo QuickSort0 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Quicksort0 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case QUICK1:
			antes_micros = micros();
			antes_millis = millis();
			quicksort1(resultado, 0, size - 1);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo QuickSort1 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Quicksort1 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case QUICK2:
			antes_micros = micros();
			antes_millis = millis();
			quicksort2(resultado, 0, size - 1);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo QuickSort2 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Quicksort2 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case QUICK3:
			antes_micros = micros();
			antes_millis = millis();
			quicksort3(resultado, 0, size - 1);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo QuickSort3 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Quicksort3 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case SEL0:
			antes_micros = micros();
			antes_millis = millis();
			selection0(resultado, size);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo Selection0 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Selection0 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case SEL1:
			antes_micros = micros();
			antes_millis = millis();
			selection1(resultado, size);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo Selection1 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Selection1 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case SEL2:
			antes_micros = micros();
			antes_millis = millis();
			selection2(resultado, size);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo Selection2 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Selection2 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case SEL3:
			antes_micros = micros();
			antes_millis = millis();
			selection3(resultado, size);
			despues_micros = micros();
			despues_millis = millis();

			(*client).print("Tiempo Selection3 en microsegundos: ");
			(*client).println(despues_micros - antes_micros);
			(*client).print("Tiempo Selection3 en milisegundos: ");
			(*client).println(despues_millis - antes_millis);
			break;

		case HELP:
			(*client).println("Comandos disponibles:\n");
			(*client).println("bubble0");
			(*client).println("bubble1");
			(*client).println("bubble2");
			(*client).println("bubble3");
			(*client).println("quick0");
			(*client).println("quick1");
			(*client).println("quick2");
			(*client).println("quick3");
			(*client).println("sel0");
			(*client).println("sel1");
			(*client).println("sel2");
			(*client).println("sel3");
			(*client).println("");
			break;

		case EXIT:
			(*client).stop();
			break;

		default:
			(*client).println("Comando invalido");
			Serial.println("Cliente desconectado");
			break;

	}
}
