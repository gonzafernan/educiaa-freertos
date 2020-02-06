/*
 * app.c
 *
 *  Created on: Feb 1, 2020
 *      Author: Gonzalo G. Fernández
 */

#include "app.h"

/*-----------------------------------------------------------*/

int main( void )
{
    /* Inicializar la placa */
    boardConfig();

    vEncoderInit();

    /* Creación de la primer tarea. */
    xTaskCreate(
        vTaskLED, /* Puntero a la función donde se
        encuentra implementada la tarea. */
        (const char *)"LED VIVO",/* Un nombre descriptivo de
        la tarea (solo como ayuda en debugueo).
         */
        configMINIMAL_STACK_SIZE*2,
        /* Tamaño del stack asignado a la tarea por el kernel 
        */
        NULL, /* Este ejemplo no recibe parámetros.
         */
        1, /* Se le asigna una prioridad 1. */
        &xLEDTaskHandle ); /* Se le pasa task handle */
    
    xTaskCreate( vTaskBuzzer, (const char*)"BUZZER",
    		configMINIMAL_STACK_SIZE, NULL, 1,
			&xBuzzerTaskHandle );

    /* Creación de la tarea asociada al encoder */
    xTaskCreate( vTaskEncoder, (const char*)"ENCODER",
        	configMINIMAL_STACK_SIZE*4, NULL,
			ENCODER_PRIORITY, &xEncoderTaskHandle );

    xTaskCreate( vTaskLCD, (const char *)"LCD init",
    		configMINIMAL_STACK_SIZE*2, NULL,
			LCD_PRIORITY, NULL );

    /* Se lanza el scheduler y comienzan a ejecutarse ambas 
     * tareas. */
    vTaskStartScheduler();
    
    /* La ejecución nunca debería llegar a este punto, salvo
     * que regrese el squeduler por quedarse sin memoria
     * disponible. */
    for( ;; );

    return 0; 
}

/*-----------------------------------------------------------*/

void vTaskLED( void *pvParameters )
{
	// Inicialización de LED vivo
	gpioInit( LED_VIVO, GPIO_OUTPUT );
	gpioWrite( LED_VIVO, ON ); // Colocar en alto, LED apagado

	/* La variable xLastWakeTime necesita inicializarse con
	 * la cuenta de ticks actual. Es la única vez que se
	 * escribe explícitamente, luego se actualiza
	 * automáticamente dentro de la API vTaskDelayUntil(). */
	TickType_t xLastWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		gpioToggle( LED_VIVO );

		/* La tarea se ejecutará exactamente cada 500ms.
		 * xLastWakeTime se actualiza autométicamente dentro
		 * de la API vTaskDelayUntil(). */
		vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 500 ) );
	}
}

/*-----------------------------------------------------------*/

void vTaskBuzzer( void *pvParameters )
{
	// Inicialización de buzzer
	gpioInit( BUZZER, GPIO_OUTPUT );
	gpioWrite( BUZZER, BUZZER_OFF );

	for( ;; )
	{
		gpioWrite( BUZZER, BUZZER_ON );
		vTaskDelay( pdMS_TO_TICKS( 100 ) );
		gpioWrite( BUZZER, BUZZER_OFF );

		vTaskDelay( pdMS_TO_TICKS( 200 ) );

		gpioWrite( BUZZER, BUZZER_ON );
		vTaskDelay( pdMS_TO_TICKS( 100 ) );
		gpioWrite( BUZZER, BUZZER_OFF );

		vTaskSuspend( NULL );
	}
}

/*-----------------------------------------------------------*/
