/*! \file uart.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

#include "stdlib.h"

#include "uart.h"

// Declaración global de cola de recepción de caracteres por UART
QueueHandle_t uartRxQueue;
// Declaración global de cola de transmisión de caracteres por UART
QueueHandle_t uartTxQueue;

/*! \var QueueHandle_t xMsgQueue
    \brief Cola de mensajes recibidos.
*/
QueueHandle_t xMsgQueue;

/*
 *  Función para comunicación del comando.
 *  Debe adaptarse a la aplicación. En este ejemplo sencillamente 
 *  se escriben todos los caracteres hasta el final de línea en la
 *  cola de transmisión.
 */
void sendCmd( char* buffer, uint8_t length )
{
	/* Asignación de memoria dinámica del mensaje */
    char *pcMsg;
    pcMsg = ( char * ) malloc( length * sizeof( char ) );
    pcMsg = buffer;
    /* Delimitador final de string */
    pcMsg[length] = '\0';

    /* Escribir caracter en cola de recepción */
	xQueueSendToBack(
		/* Handle de la cola a escribir */
		xMsgQueue,
		/* Puntero al dato a escribir */
		&pcMsg,
		/* Máximo tiempo a esperar una escritura */
		portMAX_DELAY
	);
}

/*
 *  Tarea para el procesamiento de caracteres recibidos por UART.
 */
void uartRxTask( void* pvParameters )
{
    // Índice de buffer
    uint8_t index = 0;
    // Variable que contendrá el caracter recibido
    char cRx;
    // Buffer de caracteres acumulados
    char bufferRx[50];

    BaseType_t xStatus;

    for ( ;; ) {
        /* Lectura de cola de recepción */
        xQueueReceive(
            /* Handle de la cola a leer */
            uartRxQueue,
            /* Puntero a la memoria donde guardar lectura */
            &cRx,
            /* Máximo tiempo que la tarea puede estar bloqueada
            esperando que haya información a leer */
            portMAX_DELAY // Tiempo de espera indefinido
        );
        
		if ( cRx == '\n' ) {
			// Rutina de comunicación de comando
			sendCmd( bufferRx, index );
			index = 0;
		} else {
			// Guardar dato en buffer
			bufferRx[index] = cRx;
			index++;
			if ( index >= L_BUFFER_RX ) {
				// TO DO: Warning buffer lleno (sobreescritura)
				index = 0;
			}
		}
    }
}

/*
 *  Tarea para el procesamiento de caracteres enviados por UART.
 */
void uartTxTask( void* pvParameters )
{
    // Variable que contendrá el caracter a enviar
    char cTx;
    BaseType_t xStatus;

    for ( ;; ) {
        /* Lectura de cola de recepción */
        xStatus = xQueueReceive(
            /* Handle de la cola a leer */
            uartTxQueue,
            /* Puntero a la memoria donde guardar lectura */
            &cTx,
            /* Máximo tiempo que la tarea puede estar bloqueada
            esperando que haya información a leer */
            portMAX_DELAY // Tiempo de espera indefinido
        );
        
        if ( xStatus == pdPASS ) {
            // Lectura exitosa, enviar dato
            uartWriteByte( UART_USB, cTx );
        }
    }
}

/*
 *  Rutina de interrupción en evento de recepción por UART
 */
void uartRxISR( void* pvParameters )
{
    // Detección de tarea bloqueada esperando información en la cola
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // Lectura de caracter recibido
    char cRx = uartRxRead( UART_USB );
    // Escribir caracter en cola de recepción
    xQueueSendToBackFromISR( 
        uartRxQueue, // Handle de la cola a escribit
        &cRx,        // Puntero a dato a escribir
        &xHigherPriorityTaskWoken
    );
    /* Si durante la ejecución de la API xQueueSendToBackFromISR
    una tarea abandona su estado bloqueado, y su prioridad es mayor
    que el de la tarea en estado Running, entonces 
    xHigherPriorityTaskWoken se setea a pdTRUE. Si ese es el caso,
    portYIELD_FROM_ISR solicita el cambio de contesto.
    */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*
 *  Inicialización de módulo UART para la aplicación.
 */
uint8_t uartAppInit(void)
{
    /* Inicialización de UART_USB junto con las interrupciones de Tx y Rx */
    uartConfig(UART_USB, 115200);     
    // Seteo de callback al evento de recepcion y habilitación de interrupcion
    uartCallbackSet(UART_USB, UART_RECEIVE, uartRxISR, NULL);
    // Habilitación de todas las interrupciones de UART_USB
    uartInterrupt(UART_USB, true);

    // Creación de cola de recepción
    uartRxQueue = xQueueCreate(
        L_QUEUE_RX,     // Longitud de la cola
        sizeof( char )  // Tamaño en bytes del tipo de información a guardar en la cola
    );
    
    uartTxQueue = xQueueCreate( L_QUEUE_TX, sizeof( char ) );

    xMsgQueue = xQueueCreate( 50, sizeof( char * ) );

    // Verificación de colas creadas con éxito
    if ( (uartRxQueue != NULL) && (uartTxQueue != NULL) ) {
        xTaskCreate(
            uartRxTask,                 // Funcion de la tarea a ejecutar
            (const char *)"uartRxTask", // Nombre de la tarea como String amigable para el usuario
            configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
            NULL,                       // Parametros de tarea
			priorityUartRxTask,         // Prioridad de la tarea
            NULL                        // Puntero a la tarea creada en el sistema
        );

        xTaskCreate( uartTxTask, (const char *)"uartTxTask",
            configMINIMAL_STACK_SIZE*2, NULL,
			priorityUartTxTask, NULL );
        
    } else {
        // Error al crear colas de UART
        return 1;
    }
    // Inicialización de UART con éxito
    return 0;
}
