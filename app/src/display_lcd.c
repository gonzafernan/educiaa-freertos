/*! \file display_lcd.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Septiembre 2020

    Detalle.
*/

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOSPriorities.h"
#include "task.h"

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

/* Aplicación includes */
#include "display_lcd.h"

/*! \var TaskHandle_t xDisplayTaskHandle
	\brief Handle de la tarea de control del Display LCD.
*/
TaskHandle_t xDisplayTaskHandle;

/*! \fn void vDisplayTask( void *pvParameters )
	\brief Tarea para control de display LCD.
*/
void vDisplayTask( void *pvParameters )
{

	for ( ;; ) {
		/* Esperar notificación desde encoder */
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		printf("SW PRESSED\n");
	}
}

/*! \fn BaseType_t xDisplayInit( void )
	\brief Inicialización de módulo LCD
*/
BaseType_t xDisplayInit( void )
{
	/* Inicialización de interfaz I2C */
	i2cInit( I2C0, 100000 );

	/* Esperar que se estabilice la alimentación del LCD
	 * No utilizo la API vTaskDelay por ser un proceso a
	 * ejecutar antes de lanzar el Scheduler */
	//delay( LCD_STARTUP_WAIT_MS );

	/* Inicialización del display */
	lcdInit(
			16,	/*Ancho de línea */
			2,	/* Cantidad de líneas */
			5,	/* Ancho en pixeles de un caracter */
			8	/* Altura en pixeles de un caracter */
			);

	/* Apagar cursor (no es necesario en la aplicación) */
	lcdCursorSet( LCD_CURSOR_OFF );
	/* Limíar pantalla */
	lcdClear();

	/* Colocar cursor en posición 0, 0 */
	lcdGoToXY( 0, 0 );
	/* Mensaje de inicialización */
	lcdSendStringRaw( "MyEP FIng UNCuyo" );

	/* Creación de tarea para control de display LCD */
	BaseType_t xStatus;
	xStatus = xTaskCreate(
		/* Puntero a la función que implementa la tarea */
		vDisplayTask,
		/* Nombre de la tarea amigable para el usuario */
		( const char * ) "DisplayTask",
		/* Tamaño de stack de la tarea */
		configMINIMAL_STACK_SIZE*2,
		/* Parámetros de la tarea */
		NULL,
		/* Prioridad de la tarea */
		priorityDisplayTask,
		/* Handle de la tarea creada */
		&xDisplayTaskHandle
	);

	return xStatus;
}
