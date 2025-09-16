/*Version agente del archivo rgb.cpp del GalaxyRVR*/

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
Agent RGB_Control("RGB Agent", 1, 50);
Agent Central_RGB("Central RGB Agent", 2, 50);

/*Declaraci�n de funciones de tarea*/
void centralRGB(void* pvParameters);
void rgb(void* pvParameters);

/*plataforma*/
Agent_Platform AP_RVR_RGB("windows");

/*Enum que de los tipos de instrucciones que RGB_Control puede hacer*/
typedef enum rgbIntructions : uint8_t {
	Begin,
	Write,
	Off
} rgbInstructions;

/*Struct del tipo de paquete que Central envia a RGB_Control*/
typedef struct {
	rgbInstructions instruction;
	uint32_t color;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} rgbControlPackage;


/*Comportamiento del agente de control rgb del rover*/
class  rgbBehaviour :public CyclicBehaviour {
public:
	void setup() {
		msg.add_receiver(Central_RGB.AID());
	}

	void action() {
		rgbControlPackage* central_rgb_pckg;
		msg.receive(portMAX_DELAY);
		if (msg.get_msg_type() == INFORM) {
			printf("Mensaje recibido: ");
			central_rgb_pckg = (rgbControlPackage*)msg.get_msg_content();
			printf("Instruccion: %d", central_rgb_pckg->instruction);
			printf(" | Color: %d", central_rgb_pckg->color);
			printf(" | Red: %d", central_rgb_pckg->red);
			printf(" | Green: %d", central_rgb_pckg->green);
			printf(" | Blue: %d", central_rgb_pckg->blue);
			printf("\n");

			switch (central_rgb_pckg->instruction)
			{
			case Begin:
				printf("\n-- Begin --\n");
				break;

			case Write:
				printf("\n-- Write --\n");
				break;

			case Off:
				printf("\n-- Off --\n");
				break;
			}

		}
	}

};

/*Definicion funcion del agente de control rgb*/
void rgb(void* pvParameters) {
	rgbBehaviour b;
	b.execute();
};


/*Comportamiento del agente central*/
class centralRGBBehaviour :public CyclicBehaviour {
public:
	void setup() {
		msg.add_receiver(RGB_Control.AID());
	}

	void action() {
		rgbControlPackage central_rgb_pckg;
		rgbInstructions inst = Write;
		uint32_t color = 0xFF5733; 
		uint8_t red = (color >> 16) & 0xFF;
		uint8_t green = (color >> 8) & 0xFF;
		uint8_t blue = color & 0xFF;

		central_rgb_pckg = { inst, color, red, green, blue };
		msg.set_msg_content((char*)&central_rgb_pckg);
		msg.set_msg_type(INFORM);
		msg.send();
		AP_RVR_RGB.agent_wait(pdMS_TO_TICKS(2000));
	}
};


/*Definicion funcion del agente central*/
void centralRGB(void* pvParameters) {
	centralRGBBehaviour b;
	b.execute();
}


/* ------------------------ MAIN ------------------------ */
int rgb_control_agent() {
	printf("RGB control MAES \n");
	AP_RVR_RGB.agent_init(&RGB_Control, rgb);
	printf("RGB Control Agent listo \n");
	AP_RVR_RGB.agent_init(&Central_RGB, centralRGB);
	printf("Central Agent listo \n");
	AP_RVR_RGB.boot();
	printf("boot exitoso \n");

	vTaskStartScheduler();

	for (;;); // ??

	return 0;
}