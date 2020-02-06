/*
 * encoder.c
 *
 *  Created on: Feb 1, 2020
 *      Author: Gonzalo G. Fernández
 */

#include "encoder.h"

void vEncoderInit( void )
{
	/* Configuracion de GPIO0 de la EDU-CIAA-NXP como entrada con pull-up */
	gpioConfig( ENC_SW, GPIO_INPUT_PULLUP );
	gpioConfig( ENC_DT, GPIO_INPUT_PULLUP );
	gpioConfig( ENC_CLK, GPIO_INPUT_PULLUP );

	// ---> Comienzo de funciones LPCOpen para configurar la interrupcion

	// Configure interrupt channel for the GPIO pin in SysCon block
	Chip_SCU_GPIOIntPinSel( PININT2_INDEX, GPIO2_GPIO_PORT, GPIO2_GPIO_PIN );

	//Chip_SCU_GPIOIntPinSel( PININT3_INDEX, GPIO3_GPIO_PORT, GPIO3_GPIO_PIN );

	Chip_SCU_GPIOIntPinSel( PININT4_INDEX, GPIO4_GPIO_PORT, GPIO4_GPIO_PIN );

	// Configure channel interrupt as edge sensitive and falling edge interrupt
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );

	// Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT3_INDEX) );
	// Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT3_INDEX) );
	// Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT3_INDEX) );

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT4_INDEX) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT4_INDEX) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT4_INDEX) );

	//-------------------------------------------------------
	// MANEJO DE INTERRUPCIONES: NVIC

	/* Seteo del nivel de prioridad de la interrupción */
	NVIC_SetPriority( PININT2_NVIC_NAME, 255 );

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( PININT2_NVIC_NAME );
	NVIC_EnableIRQ( PININT2_NVIC_NAME );

	//-------------------------------------------------------

	// NVIC_ClearPendingIRQ( PININT3_NVIC_NAME );
	// NVIC_EnableIRQ( PININT3_NVIC_NAME );

	/* Seteo del nivel de prioridad de la interrupción */
	NVIC_SetPriority( PININT4_NVIC_NAME, 255 );

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( PININT4_NVIC_NAME );
	NVIC_EnableIRQ( PININT4_NVIC_NAME );

	ENCODER_VALUE = 0;
}

/*-----------------------------------------------------------*/

void vTaskEncoder( void *pvParameters )
{
	// vEncoderInit();

	/* Parámetros asociados a la gestión de particiones a través
	 * de task notifications */
	const TickType_t xMaxExpectedBlockTime = portMAX_DELAY;

	uint32_t ulEventToProcess;

	for( ;; )
	{
		ulEventToProcess = ulTaskNotifyTake( pdTRUE, xMaxExpectedBlockTime );

		if( ulEventToProcess != 0 )
		{
			if( gpioRead( ENC_DT ) )
			{
				ENCODER_VALUE++;
			}
			else
			{
				ENCODER_VALUE--;
			}
		}
	}
}

/*-----------------------------------------------------------*/

/* Rutina de interrupción por switch en encoder */

void PININT2_IRQ_HANDLER( void )
{
	// Se da aviso que se trato la interrupcion
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );

	// BaseType_t xHigherPriorityTaskWoken;

	// xHigherPriorityTaskWoken = pdFALSE;

	gpioToggle( LED2 );

	// vTaskNotifyGiveFromISR( xEncoderTaskHandle, &xHigherPriorityTaskWoken );

	//portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	// portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/

void PININT4_IRQ_HANDLER( void )
{
	// Se da aviso que se trato la interrupcion
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT4_INDEX ) );

	BaseType_t xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;

	vTaskNotifyGiveFromISR( xEncoderTaskHandle, &xHigherPriorityTaskWoken );

	//portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
