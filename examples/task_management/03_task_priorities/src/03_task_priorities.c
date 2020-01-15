/*
    Ejemplo 3: Prioridad
    -----------------------------------------------------------
    El ejemplo esta basado en el ejemplo 3 del libro 
    "Masteringthe FreeRTOS™ Real Time Kernel: A Hands-On 
    Tutorial Guide" de Richard Barry, pero con una variación 
    para interactuar con los componentes de la placa de 
    desarrollo EDU-CIAA-NXP con la librería sAPI del 
    firmware_v3.

    En este caso, el ejemplo consiste en una variación del 
    ejemplo 3 "Tasks Priorities". Consiste en el ejemplo 2 
    "Task Parameter", pero con las tareas con distinta 
    prioridad.
    -----------------------------------------------------------
    Autor: Gonzalo G. Fernández
    Fecha: 15/01/2020
    Archivo: 03_task_priorities.c
    Licencia: MIT
*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

/* sAPI library include. */
#include "sapi.h"

/* Variable global para crear un delay "forzado" */
#define mainDELAY_LOOP_COUNT        ( 0xffffff )

/* Tareas a crear */
void vTaskBlinkLed( void *pvParameters );

/*-----------------------------------------------------------*/

const int iLed1 = LED1;
const int iLed2 = LED2;

int main( void )
{
    /* Inicializar la placa */
    boardConfig();

    /* Creación de la primer tarea. */
    xTaskCreate(
        vTaskBlinkLed, /* Puntero a la función donde se 
        encuentra implementada la tarea. */
        (const char *)"Blink LED1",/* Un nombre descriptivo de 
        la tarea (solo como ayuda en debugueo).
         */
        configMINIMAL_STACK_SIZE*2,
        /* Tamaño del stack asignado a la tarea por el kernel 
        */
        (void *) &iLed1, /* Este ejemplo no recibe parámetros.
         */
        1, /* Se le asigna una prioridad 1. */
        NULL ); /* No se le pasa task handle */
    
    /* Creación de la segunda tarea con mayor prioridad. */
    xTaskCreate( 
        vTaskBlinkLed, (const char *)"Blink LED2",
        configMINIMAL_STACK_SIZE*2, (void *) &iLed2, 2, NULL );
    
    /* Se lanza el scheduler y comienzan a ejecutarse ambas 
    tareas. */
    vTaskStartScheduler();
    
    /* La ejecución nunca debería llegar a este punto, salvo
     que regrese el squeduler por quedarse sin memoria 
     disponible. */
    for( ;; );

    return 0; 
}

/*-----------------------------------------------------------*/
/* Tarea que realiza un toggle de un determinado LED. */
void vTaskBlinkLed( void *pvParameters )
{
    int *piLed = (int *) pvParameters;

    volatile uint32_t ul; // Variable para loop for

    for( ;; )
    {
        gpioToggle( *piLed );

        for ( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
        {
            /* Este bucle solo provoca un delay "forzado". */
        }
    }
}