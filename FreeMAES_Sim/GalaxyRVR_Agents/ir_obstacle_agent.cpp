/*Version agente del archivo ir_obstacle.cpp del GalaxyRVR*/

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
Agent IR_Control("IR Agent", 1, 50);
Agent IR_Central("Central IR Agent", 2, 50);

/*Declaraci�n de funciones de tarea*/
void centralIR(void* pvParameters);
void ir(void* pvParameters);

/*plataforma*/
Agent_Platform AP_RVR_IR("windows");

/*Enum de los tipos de instrucciones que IR_Control puede hacer*/
typedef enum irIntructions : uint8_t {
	Begin,
	Read
} irInstructions;

/*Struct del tipo de paquete que Centra envia a IR_Control y viceversa*/
typedef struct {
	irIntructions instruction;
	uint8_t  result;				//En arduino cambiar a byte
} irControlPackage;


/*Comportamiento del agente de control ir del rover*/
class irBehaviour :public CyclicBehaviour {
public: 
	void setup() {
		msg.add_receiver(IR_Central.AID());
	}

	void action() {
		irControlPackage central_ir_pckg_o;
		irInstructions inst = Read;
		uint8_t res = 1; 
		central_ir_pckg_o = { inst, res };

		msg.set_msg_content((char*)&central_ir_pckg_o);
		msg.set_msg_type(INFORM);
		msg.send();
		AP_RVR_IR.agent_wait(pdMS_TO_TICKS(2000));

	}
};


/*Definicion de la funcion del agente de control ir*/
void ir(void* pvParameters) {
	irBehaviour b;
	b.execute();
};

/*Comportamiento del agente central*/
class centralIRBehaviour :public CyclicBehaviour {
public: 
	void setup() {
		msg.add_receiver(IR_Control.AID());
	}

	void action() {
		irControlPackage* central_ir_pckg_i;
		msg.receive(portMAX_DELAY);
		if (AP_RVR_IR.get_Agent_description(msg.get_sender()).agent_name == "IR Agent") {
			printf("Mensaje recibido: ");
			central_ir_pckg_i = (irControlPackage*)msg.get_msg_content();
			printf("Instruccion: %d", central_ir_pckg_i->instruction);
			printf(" | Resultado: %d", central_ir_pckg_i->result);
			printf("\n");
		}
	}
};

/*Definicion de la funcion del agente central*/
void centralIR(void* pvParameters) {
	centralIRBehaviour b;
	b.execute();
};

/* ------------------------ MAIN ------------------------ */
int ir_obstacle_agent() {
	printf("IR obstacle Agent MAES \n");
	AP_RVR_IR.agent_init(&IR_Control, ir);
	printf("IR Control Agent listo \n");
	AP_RVR_IR.agent_init(&IR_Central, centralIR);
	printf("Central IR Agent listo \n");
	AP_RVR_IR.boot();
	printf("boot exitoso \n");

	vTaskStartScheduler();

	for (;;); // ??

	return 0;
}