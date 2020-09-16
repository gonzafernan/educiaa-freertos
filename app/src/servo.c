/*! \file servo.c
    \brief Señal PWM, tarea y funciones para el funcionamiento
    del servomotor (Tower Pro SG90).
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020
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

/*! \var TaskHandle_t xAppSyncTaskHandle
	\brief Handle de la tarea que sincroniza mensajes.
*/
extern TaskHandle_t xAppSyncTaskHandle;

/*! \var QueueHandle_t xServoSetPointQueue
    \brief Cola de consignas recibidas a ejecutar.
*/
QueueHandle_t xServoSetPointQueue;

/*! \var QueueHandle_t xServoPositionMailbox
	\brief Handle del mailbox que contendrá la posición
	del servomotor.
*/
QueueHandle_t xServoPositionMailbox;

/*! \fn uint8_t ucServoValue( uint8_t ucAngle )
	\brief Conversión a valor de ángulo dada la discretización
	que impone el sistema.
	\param Ángulo a setear introducido por el usuario.
	\return Ángulo dada la discretización del sistema.
*/
uint8_t ucServoValue( uint8_t ucAngle )
{
	uint8_t ucMinDiff = servoANGLE_MAX / 10;
	if ( ( servoANGLE_MIN <= ucAngle ) && ( ucAngle < ucMinDiff ) ) {
		return servoANGLE_MIN;
	} else if ( ( ucMinDiff < ucAngle ) && ( ucAngle < 3*ucMinDiff ) ) {
		return 2 * ucMinDiff;
	} else if ( ( 3*ucMinDiff < ucAngle ) && ( ucAngle < 5*ucMinDiff ) ) {
		return 4 * ucMinDiff;
	} else if ( ( 5*ucMinDiff < ucAngle ) && ( ucAngle < 7*ucMinDiff ) ) {
		return 6 * ucMinDiff;
	} else if ( ( 7*ucMinDiff < ucAngle ) && ( ucAngle < 9*ucMinDiff ) ) {
		return 8 * ucMinDiff;
	} else if ( ( 9*ucMinDiff < ucAngle ) && ( ucAngle <= servoANGLE_MAX ) ) {
		return 10 * ucMinDiff;
	}
}

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

/*! \fn BaseType_t xServoAbsoluteSetPoint( uint8_t ucSetPointValue )
	\brief Setear la posición absoluta del servo.
	\param ucSetPointValue Posición a setear.
*/
BaseType_t xServoAbsoluteSetPoint( uint8_t ucSetPointValue )
{
	/* Verificación de consigna menor al mínimo admisible */
	if ( ucSetPointValue < servoANGLE_MIN ) {
		/* Setear valor mínimo admisible */
		Chip_SCTPWM_SetDutyCycle( servoSCT_PWM, servoSCT_PWM_INDEX,
			Chip_SCTPWM_PercentageToTicks( servoSCT_PWM,
				servoDUTYCYCLE_MIN ) );

		/* Actualización del valor en mailbox */
		ucSetPointValue = servoANGLE_MIN;
		xQueueOverwrite( xServoPositionMailbox, &ucSetPointValue );
		/* Devolver error */
		return pdFAIL;
	}
	/* Verificación de consigna mayor al máximo admisible */
	if ( ucSetPointValue > servoANGLE_MAX ) {
		/* Setear valor máximo admisible */
		Chip_SCTPWM_SetDutyCycle( servoSCT_PWM, servoSCT_PWM_INDEX,
			Chip_SCTPWM_PercentageToTicks( servoSCT_PWM,
				servoDUTYCYCLE_MAX ) );

		/* Actualización del valor en mailbox */
		ucSetPointValue = servoANGLE_MAX;
		xQueueOverwrite( xServoPositionMailbox, &ucSetPointValue );
		/* Devolver error */
		return pdFAIL;
	}

	Chip_SCTPWM_SetDutyCycle( servoSCT_PWM, servoSCT_PWM_INDEX,
		Chip_SCTPWM_PercentageToTicks( servoSCT_PWM,
			ucServoValue( ucSetPointValue )*5/servoANGLE_MAX + 5 ) );

	xQueueOverwrite( xServoPositionMailbox, &ucSetPointValue );

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
	uint8_t ulAngleValue = 0;

	xQueueOverwrite( xServoPositionMailbox, &ulAngleValue );

	/* Inicialización del servo en 0 */
	xServoAbsoluteSetPoint( 30 );

	for ( ;; ) {
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
		ulAngleValue = atoi( &pcReceivedSetPoint[2] );

		/* Seteo de consigna */
		if ( xServoAbsoluteSetPoint( ulAngleValue ) == pdFAIL ) {
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
				Chip_SCTPWM_PercentageToTicks( servoSCT_PWM,
					servoDUTYCYCLE_MIN ) );

	/* Lanzar el canal PWM */
	Chip_SCTPWM_Start( servoSCT_PWM );

	/* Estato para verificación de errores */
	BaseType_t xStatus;

	/* Creación de cola de consignas recibidas a ejecutar */
	xServoSetPointQueue = xQueueCreate(
		/* Longitud máxima de la cola */
		servoMAX_SETPOINT_QUEUE_LENGTH,
		/* Tamaño de elementos a guardar en cola */
		sizeof( char * )
	);
	/* Verificación de cola creada con éxito */
	if ( xServoSetPointQueue == NULL ) {
		return pdFAIL;
	}

	/* Creación de mailbox para guardar posición del motor */
	xServoPositionMailbox = xQueueCreate( 1, sizeof( uint8_t ) );
	/* Verificación de mailbox creado con éxito */
	if ( xServoPositionMailbox == NULL ) {
		return pdFAIL;
	}

	/* Creación de tarea para procesamiento asociado
	al servo motor */
	xStatus = xTaskCreate( vServoControlTask, (const char *)"ServoControlTask",
		configMINIMAL_STACK_SIZE*2, NULL,
		priorityServoControlTask, NULL );
	/* Verificación de tarea creada con éxito */
	if ( xStatus == pdFAIL ) {
		return pdFAIL;
	}

	return pdPASS;
}
