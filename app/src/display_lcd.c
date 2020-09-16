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
#include "encoder.h"
#include "stepper.h"
#include "servo.h"
#include "uart.h"

/*! \var TaskHandle_t xDisplayTaskHandle
	\brief Handle de la tarea de control del Display LCD.
*/
TaskHandle_t xDisplayTaskHandle;

/*! \fn void vUpdateSelection( uint8_t cSelection )
	\brief Actualizar selección en el displat LCD.
	\param cSel Entero con el índice de la selección.
*/
void vUpdateSelection( uint8_t cSelection )
{
	/* Colocar cursor en posición 0, 1 */
	lcdGoToXY( 0, 1 );

	switch ( cSelection ) {
	case 0:
		lcdSendStringRaw( "STEPPER 1:      " );
		printf("STEPPER 1:      \n");
		break;
	case 1:
		lcdSendStringRaw( "STEPPER 2:      " );
		printf("STEPPER 2:      \n");
		break;
	case 2:
		lcdSendStringRaw( "STEPPER 3:      " );
		printf("STEPPER 3:      \n");
		break;
	case 3:
		lcdSendStringRaw( "SERVO EXT:      " );
		printf("SERVO EXT:      \n");
		break;
	default:
		lcdSendStringRaw( " EDU-CIAA RTOS  " );
		printf("EDU-CIAA RTOS\n");
		break;
	}
}

/*! \fn void vDisplayTask( void *pvParameters )
	\brief Tarea para control de display LCD.
*/
void vDisplayTask( void *pvParameters )
{
	char* pcAngleTxt;

	/* Selección inicial de menu en display */
	vUpdateSelection( 0 );

	uint8_t cMenuSel = 0;
	uint32_t value;
	char pcAuxMsg[4];

	for ( ;; ) {
		/* Lectura del valor actual en mailbox */
		xQueuePeek( xEncoderChoiceMailbox, &cMenuSel, portMAX_DELAY );

		/* Colocar cursor en posición 0, 0 */
		lcdGoToXY( 11, 1 );

		/* Selección de motores paso a paso */
		if ( cMenuSel < stepperAPP_NUM ) {
			/* Obtener ángulo pendiente de motor */
			value = ulStepperGetAngle( cMenuSel );
			/* Escribir ángulo pendiente en formato de tres dígitos */
			sprintf( pcAuxMsg, "%03d", value );
			lcdSendStringRaw( pcAuxMsg );
		/* Selección de servomotor */
		} else if ( cMenuSel == stepperAPP_NUM ) {
			xQueuePeek( xServoPositionMailbox, &value, portMAX_DELAY );
			/* Escribir posición del motor en formato de 3 dígitos */
			sprintf( pcAuxMsg, "%03d", value );
			lcdSendStringRaw( pcAuxMsg );
		}

		vTaskDelay( pdMS_TO_TICKS( 500 ) );
	}
}

/*! \fn BaseType_t xDisplayInit( void )
	\brief Inicialización de módulo LCD
*/
BaseType_t xDisplayInit( void )
{
	/* Inicialización de interfaz I2C */
	//i2cInit( I2C0, 100000 );

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
	/* Limpiar pantalla */
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
