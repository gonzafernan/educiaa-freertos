#ifndef UART_H
#define UART_H

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOSPriorities.h"

#include "task.h"
#include "queue.h"
#include "sapi.h"

// Longitud de cola de recepción
#define L_QUEUE_RX  100
// Longitud de cola de transmisión
#define L_QUEUE_TX  100

// Tamaño de buffer de caracteres recibidos
#define L_BUFFER_RX 50

// Declaración global de cola de recepción de caracteres por UART
extern QueueHandle_t uartRxQueue;
// Declaración global de cola de transmisión de caracteres por UART
extern QueueHandle_t uartTxQueue;

extern QueueHandle_t xMsgQueue;

/*
 *  Función para comunicación del comando.
 *  Debe adaptarse a la aplicación. En este ejemplo sencillamente 
 *  se escriben todos los caracteres hasta el final de línea en la
 *  cola de transmisión.
 */
 void sendCmd( char* buffer, uint8_t length );

 /*
 *  Tarea para el procesamiento de caracteres recibidos por UART.
 */
void uartRxTask( void* pvParameters );

/*
 *  Tarea para el procesamiento de caracteres enviados por UART.
 */
void uartTxTask( void* pvParameters );

/*
 *  Rutina de interrupción en evento de recepción por UART
 */
void uartRxISR( void* pvParameters );

/*
 *  Inicialización de módulo UART para la aplicación.
 */
uint8_t uartAppInit(void);

#endif /* UART_H_ */
