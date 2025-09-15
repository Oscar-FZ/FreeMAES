/*Version agente del archivo car_control.cpp del GalaxyRVR*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// #include "maes - rtos.h"
#include "..\FreeMAES_Source\maes-rtos.h" // Puede que haya que quitar un ..\

/* Demo includes. */
#include "supporting_functions.h"
#include <iostream>

using namespace MAES;
using namespace std;

/*Agentes*/
Agent Car_Control("Car Agent", 1, 50);
Agent Central("Central Agent", 2, 50);

/*Declaraci�n de funciones de tarea*/
void control(void* pvParameters);
void central(void* pvParameters);

/*plataforma*/
Agent_Platform AP_RVR("windows");

/*Enum que de los tipos de instrucciones que Car_Control puede hacer*/
typedef enum carInstructions : uint8_t {
	Begin,
	Forward,
	Backward,
	TurnLeft,
	TurnRight,
	Stop,
	SetMotors
} carInstructions;

/*Struct del tipo de paquete que Central envia a Car_Control*/
typedef struct {
	carInstructions instruction;
	int8_t power1;
	int8_t power2;
} carControlPackage;

void carSetMotors(int8_t power0, int8_t power1) {
	/* Implementacion del Simulador*/
	printf("Set motors: %d , %d\n", power0, power1);

	/* Implementacion del Arduino */
	/*bool dir[2];
	int8_t power[2] = { power0, power1 };
	int8_t newPower[2];

	for (uint8_t i = 0; i < 2; i++) {
		dir[i] = power[i] > 0;

		if (MOTOR_DIRECTIONS[i]) dir[i] = !dir[i];

		if (power[i] == 0) {
			newPower[i] = 0;
		}
		else {
			newPower[i] = map(abs(power[i]), 0, 100, MOTOR_POWER_MIN, 255);
		}
		#if defined(ARDUINO_AVR_UNO)
			SoftPWMSet(MOTOR_PINS[i * 2], dir[i] * newPower[i]);
			SoftPWMSet(MOTOR_PINS[i * 2 + 1], !dir[i] * newPower[i]);
		#elif defined(ARDUINO_MINIMA)
			analogWrite(MOTOR_PINS[i * 2], dir[i] * newPower[i]);
			analogWrite(MOTOR_PINS[i * 2 + 1], !dir[i] * newPower[i]);
		#endif
	}*/
};


/*Comportamiento del agente de control del carro*/
class  carBehaviour :public CyclicBehaviour {
public:
	void setup() {
		msg.add_receiver(Central.AID());
	}

	void action() {
		printf(AP_RVR.get_Agent_description(AP_RVR.get_running_agent()).agent_name);
		printf(": Controlando el carro...\n");

		while (true) {
			carControlPackage* central_car_pckg;
			msg.receive(portMAX_DELAY);
			if (msg.get_msg_type() == INFORM) {
				printf("Mensaje recibido: ");
				central_car_pckg = (carControlPackage*)msg.get_msg_content();
				printf("Instruccion: %d", central_car_pckg->instruction);
				printf(" | Valor1: %d", central_car_pckg->power1);
				printf(" | Valor2: %d", central_car_pckg->power2);
				printf("\n");

				switch (central_car_pckg->instruction) 
				{
					case Begin: 
						printf("\n-- Begin --\n");
						break;

					case Forward: 
						printf("\n-- Forward --\n");
						carSetMotors(central_car_pckg->power1, -(central_car_pckg->power1));
						break;

					case Backward: 
						printf("\n-- Backward --\n");
						carSetMotors(central_car_pckg->power1, -(central_car_pckg->power1));
						break;

					case TurnLeft: 
						printf("\n-- Turn Left --\n");
						carSetMotors(central_car_pckg->power1, -(central_car_pckg->power1));
						break;

					case TurnRight: 
						printf("\n-- Turn Right --\n");
						carSetMotors(central_car_pckg->power1, -(central_car_pckg->power1));
						break;

					case Stop: 
						printf("\n-- Stop --\n");
						carSetMotors(0, 0);
						break;

					case SetMotors: 
						printf("\n-- Set Motors --\n");
						carSetMotors(central_car_pckg->power1, central_car_pckg->power2);
						break;

					default: 
						printf("Instruccion no reconocida\n"); 
						break;

				}
			}
		}		
	}
};

/*Definicion funcion del carro*/
void control(void* pvParameters) {
	carBehaviour b;
	b.execute();
}


/*Comportamiento del agente central*/
class centralBehaviour :public CyclicBehaviour {
public:
	void setup() {
		msg.add_receiver(Car_Control.AID());
	}

	void action() {

		carControlPackage central_car_pckg;
		carInstructions inst = Forward;
		int8_t power1 = -8;
		int8_t power2 = 89;
		central_car_pckg = {inst, power1, power2};
		msg.set_msg_content((char*)&central_car_pckg);
		msg.set_msg_type(INFORM);
		msg.send();
		AP_RVR.agent_wait(pdMS_TO_TICKS(2000));
	}
};

/*Definicion funcion del agente central*/
void central(void* pvParameters) {
	centralBehaviour b;
	b.execute();
}


/* ------------------------ MAIN ------------------------ */
int car_control_agent() {
	printf("Car control Agent MAES \n");
	AP_RVR.agent_init(&Car_Control, control);
	printf("Car Control Agent listo \n");
	AP_RVR.agent_init(&Central, central);
	printf("Central Agent listo \n");
	AP_RVR.boot();
	printf("boot exitoso \n");

	vTaskStartScheduler();

	for (;;); // ??

	return 0;
}