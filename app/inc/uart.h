/*
 * uart.h
 *
 *  Created on: Feb 9, 2020
 *      Author: Gonzalo G. Fernández
 */

#ifndef UART_H
#define UART_H

/*-----------------------------------------------------------*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "sapi.h"
#include "chip.h"

/*-----------------------------------------------------------*/

#define UART_BAUDRATE	115200

void vUARTRxHandler( void *pvParameters );
void vUARTInit( void );

/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

#endif /* UART_H */
