/*! \file uart.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

#include "uart.h"

/*! \var QueueHandle_t xUartTxQueue
	\brief Declaración global de cola de recepción de
	caracteres por UART
*/
QueueHandle_t xUartRxQueue;

/*! \var QueueHandle_t xUartTxQueue
	\brief Declaración global de cola de transmisión de
	caracteres por UART
*/
QueueHandle_t xUartTxQueue;

/*! \var QueueHandle_t xMsgQueue
    \brief Cola de mensajes recibidos.
*/
extern QueueHandle_t xMsgQueue;

/*! \var SemaphoreHandle_t xUartTxMutex
	\brief Mutex para la protección de la cola de transmisión.
*/
SemaphoreHandle_t xUartTxMutex;

/*! \fn void vUartSendMsg( char *pcMsg )
	\brief Enviar mensaje a la cola de transmisión.
	\param pcMsg Puntero al string mensaje.
*/
void vUartSendMsg( char *pcMsg )
{
	/* Tomar mutex de cola de transmisión */
	xSemaphoreTake( xUartTxMutex, portMAX_DELAY );
	/* Escribir mensaje en cola de transmisión */
	xQueueSendToBack(
		/* Handle de la cola a escribir */
		xUartTxQueue,
		/* Puntero al dato a escribir */
		&pcMsg,
		/* Máximo tiempo a esperar una escritura */
		portMAX_DELAY
	);
	/* Liberar mutex */
	xSemaphoreGive( xUartTxMutex );
}

/*! \fn void vSendCmd( char* pcBuffer, uint8_t cLength )
	\brief Enviar comando a cola de mensajes recibidos.
	\param pcBuffer Puntero al inicio del buffer.
	\param cLength Longitud del buffer (cantidad de caracteres).
*/
void vSendCmd( char* pcBuffer, uint8_t cLength )
{
	/* Asignación de memoria dinámica del mensaje */
    char *pcMsg;
    pcMsg = ( char * ) pvPortMalloc( cLength * sizeof( char ) );
    pcMsg = pcBuffer;
    /* Delimitador final de string */
    pcMsg[cLength] = '\0';

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

/*! \fn void vUartRxTask( void* pvParameters )
	\brief Tarea para el procesamiento de caracteres
	recibidos por UART.
*/
void vUartRxTask( void* pvParameters )
{
    // Índice de buffer
    uint8_t cIndex = 0;
    // Variable que contendrá el caracter recibido
    char cRx;
    // Buffer de caracteres acumulados
    char pcBufferRx[50];

    BaseType_t xStatus;

    for ( ;; ) {
        /* Lectura de cola de recepción */
        xQueueReceive(
            /* Handle de la cola a leer */
            xUartRxQueue,
            /* Puntero a la memoria donde guardar lectura */
            &cRx,
            /* Máximo tiempo que la tarea puede estar bloqueada
            esperando que haya información a leer */
            portMAX_DELAY // Tiempo de espera indefinido
        );
        
		if ( cRx == '\n' ) {
			// Rutina de comunicación de comando
			vSendCmd( pcBufferRx, cIndex );
			cIndex = 0;
		} else {
			// Guardar dato en buffer
			pcBufferRx[cIndex] = cRx;
			cIndex++;
			if ( cIndex >= uartBUFFER_RX_LENGTH ) {
				// TO DO: Warning buffer lleno (sobreescritura)
				cIndex = 0;
			}
		}
    }
}

/*! \fn void vUartTxTask( void* pvParameters )
	\brief Tarea para el procesamiento de caracteres
	enviados por UART.
*/
void vUartTxTask( void* pvParameters )
{
    /* Variable que contendrá el string a enviar */
    char *pcMsgToSend;

    for ( ;; ) {
        /* Lectura de cola de recepción */
        xQueueReceive(
            /* Handle de la cola a leer */
            xUartTxQueue,
            /* Puntero a la memoria donde guardar lectura */
            &pcMsgToSend,
            /* Máximo tiempo que la tarea puede estar bloqueada
            esperando que haya información a leer */
            portMAX_DELAY // Tiempo de espera indefinido
        );
        
        /* Envío del mensaje */
        printf( "%s\n", pcMsgToSend );
    }
}

/*! \fn void vUartRxISR( void* pvParameters )
	\brief Rutina de interrupción en evento de recepción por UART.
*/
void vUartRxISR( void* pvParameters )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /* Lectura de caracter recibido */
    char cRx = uartRxRead( UART_USB );
    /* Escribir caracter en cola de recepción */
    xQueueSendToBackFromISR(
    	/* Handle de la cola a escribir */
        xUartRxQueue,
		/* Puntero a dato a escribir */
        &cRx,
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

/*! \fn BaseType_t uartAppInit(void)
	\brief Inicialización de módulo UART con sus respectivas colas.
*/
BaseType_t xUartInit( void )
{
    /* Inicialización de UART_USB junto con las interrupciones de Tx y Rx */
    uartConfig(UART_USB, 115200);     
    /* Seteo de callback al evento de recepcion y habilitación de interrupcion */
    uartCallbackSet(UART_USB, UART_RECEIVE, vUartRxISR, NULL);
    /* Habilitación de todas las interrupciones de UART_USB */
    uartInterrupt(UART_USB, true);

    /* Creación de cola de recepción */
    xUartRxQueue = xQueueCreate(
		/* Longitud de la cola */
		uartQUEUE_RX_LENGTH,
		/* Tamaño en bytes del tipo de información a guardar en la cola */
        sizeof( char )
    );
    
    /* Creación de cola de mensajes a enviar */
    xUartTxQueue = xQueueCreate( uartQUEUE_TX_LENGTH, sizeof( char * ) );

    /* Creación de mutex para cola de transmisión */
    xUartTxMutex = xSemaphoreCreateMutex();

    /* Verificación de colas creadas con éxito */
    if ( (xUartRxQueue != NULL) && ( xUartTxQueue != NULL ) ) {
        /* Creación de tarea gatekeeper para recepción */
    	xTaskCreate(
            vUartRxTask,                 // Funcion de la tarea a ejecutar
            (const char *)"UartRxTask", // Nombre de la tarea como String amigable para el usuario
            configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
            NULL,                       // Parametros de tarea
			priorityUartRxTask,         // Prioridad de la tarea
            NULL                        // Puntero a la tarea creada en el sistema
        );
    	/* Creación de tarea gatekeeper para transmisión */
        xTaskCreate( vUartTxTask, (const char *)"UartTxTask",
            configMINIMAL_STACK_SIZE*2, NULL,
			priorityUartTxTask, NULL );
        
    } else {
        /* Error al crear colas de UART */
        return pdFAIL;
    }
    /* Inicialización de UART con éxito */
    return pdPASS;
}
