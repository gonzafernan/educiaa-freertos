#ifndef UART_H
#define UART_H

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOSPriorities.h"
#include "task.h"
#include "queue.h"

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

/*! \def uartQUEUE_RX_LENGTH
	\brief Longitud de cola de recepción.
*/
#define uartQUEUE_RX_LENGTH  100

/*! \def uartQUEUE_TX_LENGTH
	\brief Longitud de cola de transmisión.
*/
#define uartQUEUE_TX_LENGTH  100

/*! \def uartBUFFER_RX_LENGTH
	\brief Tamaño de buffer de caracteres recibidos.
*/
#define uartBUFFER_RX_LENGTH 50

/*! \fn void vUartSendMsg( char *pcMsg )
	\brief Enviar mensaje a la cola de transmisión.
*/
void vUartSendMsg( char *pcMsg );

/*! \fn BaseType_t uartAppInit(void)
	\brief Inicialización de módulo UART con sus respectivas colas.
*/
BaseType_t xUartInit( void );

#endif /* UART_H_ */
