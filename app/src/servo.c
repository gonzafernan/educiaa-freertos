/*! \file servo.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

/* Utilidades includes */
#include <stdlib.h>

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "FreeRTOSPriorities.h"
#include "task.h"
#include "queue.h"

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"
#include "chip.h"

/* Aplicación includes */
#include "servo.h"
#include "uart.h"

/*! \var TaskHandle_t xAppSyncTaskHandle
	\brief Handle de la tarea que sincroniza mensajes.
*/
extern TaskHandle_t xAppSyncTaskHandle;

/*! \var QueueHandle_t xServoSetPointQueue
    \brief Cola de consignas recibidas a ejecutar.
*/
QueueHandle_t xServoSetPointQueue;

/*! \fn void vServoSendMsg( char *pcMsg )
	\brief Enviar consigna a cola de consignas pendientes.
	\param pcMsg String con consigna a enviar.
*/
void vServoSendMsg( char *pcMsg)
{
	/* Escribir mensaje en cola de transmisión */
	xQueueSendToBack(
		/* Handle de la cola a escribir */
		xServoSetPointQueue,
		/* Puntero al dato a escribir */
		&pcMsg,
		/* Máximo tiempo a esperar una escritura */
		portMAX_DELAY
	);
}

/*! \fn void vServoStop( void )
	\brief Detener señal PWM al servo.
*/
void vServoStop( void )
{
	Chip_SCTPWM_Stop( servoSCT_PWM );
}

/*! \fn BaseType_t xServoAbsoluteSetPoint( int8_t iSetPointValue )
	\brief Setear la posición absoluta del servo.
	\param iSetPointValue Posición a setear.
*/
BaseType_t xServoAbsoluteSetPoint( int8_t iSetPointValue )
{
	/* Verificación de consigna menor al mínimo admisible */
	if ( iSetPointValue < servoANGLE_MIN ) {
		/* Setear valor mínimo admisible */
		Chip_SCTPWM_SetDutyCycle( servoSCT_PWM, servoSCT_PWM_INDEX,
			Chip_SCTPWM_PercentageToTicks( servoSCT_PWM,
				servoDUTYCYCLE_MIN ) );
		/* Devolver error */
		return pdFAIL;
	}
	/* Verificación de consigna mayor al máximo admisible */
	if ( iSetPointValue > servoANGLE_MAX ) {
		/* Setear valor máximo admisible */
		Chip_SCTPWM_SetDutyCycle( servoSCT_PWM, servoSCT_PWM_INDEX,
			Chip_SCTPWM_PercentageToTicks( servoSCT_PWM,
				servoDUTYCYCLE_MAX ) );
		/* Devolver error */
		return pdFAIL;
	}
	/* Verificación de consigna 0 */
	if ( iSetPointValue == 0 ) {
		/* Setear valor 0 */
		Chip_SCTPWM_SetDutyCycle( servoSCT_PWM, servoSCT_PWM_INDEX,
			Chip_SCTPWM_PercentageToTicks( servoSCT_PWM,
				( servoDUTYCYCLE_MAX + servoDUTYCYCLE_MIN ) / 2 ) );
		return pdPASS;
	}
	return pdPASS;
}

/*! \fn void vServoControlTask( void *pvParameters )
	\brief Tarea para procesamiento asociado
	al servo motor
*/
void vServoControlTask( void *pvParameters )
{
	/* Puntero a consignas recibidas */
	char *pcReceivedSetPoint;

	/* Valor de ángulo a setear */
	int8_t iAngleValue;

	/* Inicialización del servo en 0 */
	xServoAbsoluteSetPoint( 0 );

	for ( ;; ) {
		printf("SERVO");
		/* Lectura de cola de consignas */
		xQueueReceive(
			/* Handle de la cola a leer */
			xServoSetPointQueue,
			/* Elemento donde guardar información leída */
			&pcReceivedSetPoint,
			/* Máxima cantidad de tiempo a esperar por una lectura */
			portMAX_DELAY
		);

		/* Lectura de ID del motor a setear */
		iAngleValue = atoi( &pcReceivedSetPoint[2] );

		/* Seteo de consigna */
		if ( xServoAbsoluteSetPoint( iAngleValue ) == pdFAIL ) {
			/* Error en ángulo */
			xTaskNotify( xAppSyncTaskHandle,
				( 1 << servoERROR_NOTIF_ANG ), eSetBits );
		}
	}
}

/*! \fn BaseType_t xServoInit( void )
	\brief Inicialización de módulo asociado a servomotor.
*/
BaseType_t xServoInit( void )
{
	/* Inicializar el SCT (State Configurable Timer) */
	Chip_SCTPWM_Init( servoSCT_PWM );

	/* Setear frecuencia del SCT */
	Chip_SCTPWM_SetRate( servoSCT_PWM, servoSCT_PWM_RATE );

	/* Setear como salida el pin de salida específico */
	Chip_SCU_PinMuxSet( servoSCU_PORT, servoSCU_PIN,
		( SCU_MODE_INACT | SCU_MODE_FUNC1 ) );

	/* Asignar un pin de salida PWM a un canal */
	Chip_SCTPWM_SetOutPin( servoSCT_PWM, servoSCT_PWM_INDEX,
		servoSCT_PWM_PIN );

	/* Setear el duty-cycle para la salida de PWM
	 * generada en el canal */
	Chip_SCTPWM_SetDutyCycle( servoSCT_PWM, servoSCT_PWM_INDEX,
		( servoDUTYCYCLE_MAX + servoDUTYCYCLE_MIN ) / 2 );

	/* Lanzar el canal PWM */
	Chip_SCTPWM_Start( servoSCT_PWM );

	/* Creación de cola de consignas recibidas a ejecutar */
	xServoSetPointQueue = xQueueCreate(
		/* Longitud máxima de la cola */
		servoMAX_SETPOINT_QUEUE_LENGTH,
		/* Tamaño de elementos a guardar en cola */
		sizeof( char * )
	);
	/* Verificación de cola creada con éxito */
	configASSERT( xServoSetPointQueue != NULL );

	/* Creación de tarea para procesamiento asociado
	al servo motor */
	xTaskCreate( vServoControlTask, (const char *)"ServoControlTask",
		configMINIMAL_STACK_SIZE*2, NULL,
		priorityServoControlTask, NULL );

	return pdPASS;
}
