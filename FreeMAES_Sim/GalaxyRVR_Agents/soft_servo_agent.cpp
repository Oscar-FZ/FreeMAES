/*Version agente del archivo soft_servo.cpp del GalaxyRVR*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// #include "maes - rtos.h"
#include "..\FreeMAES_Source\maes-rtos.h" 

/* Demo includes. */
#include "supporting_functions.h"
#include <iostream>

using namespace MAES;
using namespace std;

/*Agentes*/
Agent Servo_Control("Servo Agent", 1, 50);
Agent Servo_Central("Servo Central Agent", 2, 50);

/*Declaraci�n de funciones de tarea*/
void servo(void* pvParameters);
void centralServo(void* pvParameters);


/*plataforma*/
Agent_Platform AP_RVR_SERVO("windows");

/*Enum que de los tipos de instrucciones que Servo_Control puede hacer*/
typedef enum servoInstructions : uint8_t {
	Begin,
	Attach,
	Write
} servoInstructions;

/*Struct del tipo de paquete que Central envia a Servo_Control*/
typedef struct {
	servoInstructions instruction;
	uint8_t pin;
	uint8_t angle;
} servoControlPackage;

/*Comportamiento del agente de control del servo del rover*/
class servoBehaviour :public CyclicBehaviour {
public:
	void setup() {
		msg.add_receiver(Servo_Central.AID());
	}

	void action() {
		servoControlPackage* central_servo_pckg;
		msg.receive(portMAX_DELAY);
		if (msg.get_msg_type() == INFORM) {
			printf("Mensaje recibido: ");
			central_servo_pckg = (servoControlPackage*)msg.get_msg_content();
			printf("Instruccion: %d", central_servo_pckg->instruction);
			printf(" | Pin: %d", central_servo_pckg->pin);
			printf(" | Angle: %d", central_servo_pckg->angle);
			printf("\n");

			switch (central_servo_pckg->instruction)
			{
				case Begin:
					printf("\n-- Begin --\n");
					break;

				case Attach:
					printf("\n-- Attach --\n");
					break;

				case Write:
					printf("\n-- Write --\n");
					break;

			}
		}
	}
};

/*Definicion funcion del agente de control del servo*/
void servo(void* pvParameters) {
	servoBehaviour b;
	b.execute();
};

/*Comportamiento del agente central*/
class centralServoBehaviour :public CyclicBehaviour {
public:
	void setup() {
		msg.add_receiver(Servo_Control.AID());
	}

	void action() {
		servoControlPackage central_servo_pckg;
		servoInstructions inst = Attach;
		uint8_t pin = 6;
		uint8_t angle = 90;
		central_servo_pckg = { inst, pin, angle };
		msg.set_msg_content((char*)&central_servo_pckg);
		msg.set_msg_type(INFORM);
		msg.send();
		AP_RVR_SERVO.agent_wait(pdMS_TO_TICKS(2000));
	}
};

/*Definicion funcion del agente central*/
void centralServo(void* pvParameters) {
	centralServoBehaviour b;
	b.execute();
};

/* ------------------------ MAIN ------------------------ */
int servo_control_agent() {
	printf("Servo control MAES\n");
	AP_RVR_SERVO.agent_init(&Servo_Control, servo);
	printf("Servo Agent listo\n");
	AP_RVR_SERVO.agent_init(&Servo_Central, centralServo);
	printf("Servo Central Agent listo\n");
	AP_RVR_SERVO.boot();
	printf("boot exitoso \n");

	vTaskStartScheduler();

	for (;;);

	return 0;
}