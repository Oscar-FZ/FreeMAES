/*Version agente del archivo car_control.cpp del GalaxyRVR*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// #include "maes - rtos.h"
#include "..\FreeMAES_Source\maes-rtos.h" // Puede que haya que quitar un ..\

/* Demo includes. */
#include "supporting_functions.h"

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


typedef enum {
	carBegin,
	carForward,
	carBackward,
	carTurnLeft,
	carTurnRight,
	carStop,
	carSetMotors
} carInstructions;


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
			msg.receive(portMAX_DELAY);
			if (msg.get_msg_type() == INFORM) {
				printf("Mensaje recibido: ");
				//printf(msg.get_msg_content());
				printf("\n");

				switch ((int)msg.get_msg_content()) 
				{
					case carBegin: 
						printf("\n-- Begin --\n");
						break;

					case carForward: 
						printf("\n-- Forward --\n");
						break;

					case carBackward: 
						printf("\n-- Backward --\n");
						break;

					case carTurnLeft: 
						printf("\n-- Turn Left --\n");
						break;

					case carTurnRight: 
						printf("\n-- Turn Right --\n");
						break;

					case carStop: 
						printf("\n-- Stop --\n");
						break;

					case carSetMotors: 
						printf("\n-- Set Motors --\n");
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
		carInstructions inst = carSetMotors;
		msg.set_msg_content((char*)inst);
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