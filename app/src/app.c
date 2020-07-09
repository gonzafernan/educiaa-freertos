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

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

/* Aplicación includes */
#include "uart.h"
#include "stepper.h"

void vAppSyncTask( void *pvParameters )
{
    for ( ;; ) {
        
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
        tskIDLE_PRIORITY+5,
        /* Handle de la tarea creada */
        NULL
    );

    // Inicialización de Scheduler
    vTaskStartScheduler();

    for ( ;; );

    return 0;
}