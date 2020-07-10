/*! \file app.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOSPriorities.h"

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

/* Aplicación includes */
#include "uart.h"
#include "stepper.h"

void vAppSyncTask( void *pvParameters )
{
    /* Puntero al mensaje leído */
    char *pcMsgReceived;

    for ( ;; ) {
        xQueueReceive(
            /* Handle de la cola a leer */
            xMsgQueue,
            /* Puntero a la memoria donde guardar lectura */
            &pcMsgReceived,
            /* Máximo tiempo que la tarea puede estar bloqueada
            esperando que haya información a leer */
            portMAX_DELAY
        );

		printf( "AppSync:%s\n", pcMsgReceived );

		/* Escribir caracter en cola de consignas */
		xQueueSendToBack(
			/* Handle de la cola a escribir */
			xStepperSetPointQueue,
			/* Puntero al dato a escribir */
			&pcMsgReceived,
			/* Máximo tiempo a esperar una escritura */
			portMAX_DELAY
		);
    }
}

uint8_t main( void )
{
    /* Inicialización de la placa */
    boardConfig();

    /* Flags de estado de los diferentes módulos */
    BaseType_t xUartStatus, xStepperStatus;
    /* Inicialización de UART */

    /* Inicialización de motor stepper */
    xStepperStatus = xStepperInit();

    // Inicialización de UART
    if ( uartAppInit() ) {
        // TO DO: Warning con LED
    }

    /* Creación de tarea de control de flujo de trabajo del programa */
    BaseType_t xStatus;
    xStatus = xTaskCreate(
        /* Puntero a la función que implementa la tarea */
        vAppSyncTask,
        /* Nombre de la tarea amigable para el usuario */
        ( const char * ) "AppSyncTask",
        /* Tamaño de stack de la tarea */
        configMINIMAL_STACK_SIZE*2,
        /* Parámetros de la tarea */
        NULL,
        /* Prioridad de la tarea */
		priorityAppSyncTask,
        /* Handle de la tarea creada */
        NULL
    );

    // Inicialización de Scheduler
    vTaskStartScheduler();

    for ( ;; );

    return 0;
}
