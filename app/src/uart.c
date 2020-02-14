/*
 * @brief Funciones asociadas a la comunicación UART
 *
 * @note
 * Las librería dependencia es la sAPI
 *
 * uart.c
 *
 *  Created on: Feb 9, 2020
 *      Author: Gonzalo G. Fernández
 */

#include "uart.h"

/*-----------------------------------------------------------*/

//void  UART2_IRQHandler( void )
void vUARTRxHandler( void *pvParameters )
{
	BaseType_t xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;

	gpioToggle( LED3 );

	//Chip_UART_SendBlocking(LPC_USART2 ,"Hola!\n\r", 7);

	char c = uartRxRead( UART_USB );
	printf( "Recibimos <<%c>> por UART\r\n", c );

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void vUARTInit( void )
{
	/* Inicialización de la UART (comunicación a través de
	 * USB DEBUG) */
	//Chip_UART_Init( LPC_USART2 );

	uartConfig(UART_USB, 115200);

	/*
	Chip_UART_ConfigData( LPC_USART2,
			UART_LCR_WLEN8 | UART_LCR_SBS_1BIT |
			UART_LCR_PARITY_DIS );
			*/

	uartCallbackSet(UART_USB, UART_RECEIVE, vUARTRxHandler, NULL);

	/* Seteo de la velocidad de comunicación (baudrate) */
	//Chip_UART_SetBaud( LPC_USART2, 115200 );

	//Chip_UART_IntEnable( LPC_USART2, UART_IER_RBRINT );

	uartInterrupt(UART_USB, true);

	/* Seteo del nivel de prioridad de la interrupción */
	NVIC_SetPriority( USART2_IRQn, 255 );

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( USART2_IRQn );
	NVIC_EnableIRQ( USART2_IRQn );

	//Chip_UART_TXEnable( LPC_USART2 );
}

