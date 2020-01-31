#include "app.h"

/*-----------------------------------------------------------*/

int main( void )
{
    /* Inicializar la placa */
    boardConfig();

    // Inicialización de LED vivo
    gpioInit( LED_VIVO, GPIO_OUTPUT );

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
        NULL ); /* No se le pasa task handle */
    
    xTaskCreate( vTaskLCDInit, (const char *)"LCD init",
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

