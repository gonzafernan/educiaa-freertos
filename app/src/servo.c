/*
 * @brief Funciones asociadas a la manipulación del servomotor
 * a través de la configuracion del SCT (State Configurable Timer)
 * para salida PWM.
 *
 * @note
 * No se utilizó las funciones específicas para motor servo
 * disponibles en la sAPI del firmware_v3 de la placa de desarrollo EDU_CIAA_NXP por
 * no ser compatibles con freeRTOS.
 * Las librerías dependencias son sAPI y LPCOpen
 *
 * servo.c
 *
 *  Created on: Feb 5, 2020
 *      Author: Gonzalo G. Fernández
 */

#include "servo.h"

/**
 * @brief	Configuración para inicializar el SCT como
 * 			salida PWM y creación de tarea RTOS vinculada
 * 			al servo motor
 * @param	None
 * @return	None
 */
void vServoInit( void )
{
	/* Inicializar el SCT (State Configurable Timer) */
	Chip_SCTPWM_Init( SCT_PWM );

	/* Setear frecuencia del SCT */
	Chip_SCTPWM_SetRate( SCT_PWM, SCT_PWM_RATE );

	/* Setear como salida el pin de salida específico */
	Chip_SCU_PinMuxSet( SCU_PORT_SERVO, SCU_PIN_SERVO,
			( SCU_MODE_INACT | SCU_MODE_FUNC1 ) );

	/* Asignar un pin de salida PWM a un canal */
	Chip_SCTPWM_SetOutPin( SCT_PWM, SCT_PWM_SERVO,
			SCT_PWM_PIN_SERVO );

	/* Setear el duty-cycle para la salida de PWM
	 * generada en el canal */
	Chip_SCTPWM_SetDutyCycle( SCT_PWM, SCT_PWM_SERVO,
			( SERVO_DUTYCYCLE_MAX + SERVO_DUTYCYCLE_MIN ) / 2 ); // duty-cycle inicializado en 0

	/* Lanzar el canal PWM */
	Chip_SCTPWM_Start( SCT_PWM );

	SERVO_VALUE = 0;

	/* Creación de tarea para procesamiento asociado
	 * al servo motor */
	xTaskCreate( vTaskServo, (const char *)"SERVO",
	        	configMINIMAL_STACK_SIZE*2, NULL,
	    		SERVO_PRIORITY, &xServoTaskHandle );
}

/**
 * @brief	Detiene la salida PWM y el SCT en general
 * @param	None
 * @return	None
 */
void vServoStop( void )
{
	Chip_SCTPWM_Stop( SCT_PWM );
}

/**
 * @brief	Setear la posición del servo motor
 * @param	ilValue	: El ángulo consigna del motor
 * @return	-1 si la consigna se encontraba fuera de rango,
 * 			0 si la consigna se encontraba dentro de rango
 * @note	El rango de funcionamiento del motor es para
 * 			ángulos entre SERVO_ANGLE_MIN y SERVO_ANGLE_MAX,
 * 			de pasar un ángulo fuera de este rango, se seteará
 * 			por defecto el valor extremo más cercano.
 *
 */
int8_t isServoSetPosition( int32_t ilValue )
{
	int32_t ilPosition;

	ilPosition = ilValue;

	/* Resolución de casos específicos para simplificar
	 * procesamiento */

	if( ilValue < SERVO_ANGLE_MIN )
	{
		SERVO_VALUE = SERVO_ANGLE_MIN;

		Chip_SCTPWM_SetDutyCycle( SCT_PWM, SCT_PWM_SERVO,
				Chip_SCTPWM_PercentageToTicks( SCT_PWM,
						SERVO_DUTYCYCLE_MIN ) );

		// WARNING valor fuera de rango
		return -1;
	}
	else if( SERVO_ANGLE_MAX < ilValue )
	{
		SERVO_VALUE = SERVO_ANGLE_MAX;

		Chip_SCTPWM_SetDutyCycle( SCT_PWM, SCT_PWM_SERVO,
				Chip_SCTPWM_PercentageToTicks( SCT_PWM,
						SERVO_DUTYCYCLE_MAX ) );

		// WARNING valor fuera de rango
		return -1;
	}
	else if( ilValue == 0)
	{
		SERVO_VALUE = 0;

		Chip_SCTPWM_SetDutyCycle( SCT_PWM, SCT_PWM_SERVO,
				Chip_SCTPWM_PercentageToTicks( SCT_PWM,
						( SERVO_DUTYCYCLE_MAX + SERVO_DUTYCYCLE_MIN ) / 2 )
						);

		return 0;
	}

	/* Conversión de ángulo a duty-cycle */
	uint32_t ulPercentage;

	ulPercentage = ( SERVO_DUTYCYCLE_MAX - SERVO_DUTYCYCLE_MIN ) * 1000;
	ulPercentage = ( ulPercentage / ( SERVO_ANGLE_MAX - SERVO_ANGLE_MIN ) ) * ilPosition;
	ulPercentage =  ulPercentage + ( SERVO_DUTYCYCLE_MAX + SERVO_DUTYCYCLE_MIN ) * 500;

	uint8_t usValue = ulPercentage/1000;

	Chip_SCTPWM_SetDutyCycle( SCT_PWM, SCT_PWM_SERVO,
			Chip_SCTPWM_PercentageToTicks( SCT_PWM, usValue ) );

	return 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief	freeRTOS Task asociada al control del servomotor
 * @param	None
 * @return	None
 */
void vTaskServo( void *pvParameters )
{
	/* Parámetros asociados a la gestión de interrupciones a
	 * través de task notifications */
	const TickType_t xMaxExpectedBlockTime = portMAX_DELAY;

	uint32_t ulEventToProcess;
	int8_t flag;

	for( ;; )
	{
		ulEventToProcess = ulTaskNotifyTake( pdTRUE, xMaxExpectedBlockTime );

		if( ulEventToProcess != 0 )
		{
			flag = isServoSetPosition( (int32_t) ENCODER_VALUE );
		}
	}
}

/*-----------------------------------------------------------*/

