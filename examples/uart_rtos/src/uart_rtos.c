/*

You need to add

DEFINES+=TICK_OVER_RTOS
DEFINES+=USE_FREERTOS

on config.mk to tell SAPI to use FreeRTOS Systick

TO DO: Implementación de warnings mediante LEDs

*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
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
QueueHandle_t uartRxQueue;
// Declaración global de cola de transmisión de caracteres por UART
QueueHandle_t uartTxQueue;

/*
 *  Función para comunicación del comando.
 *  Debe adaptarse a la aplicación. En este ejemplo sencillamente 
 *  se escriben todos los caracteres hasta el final de línea en la
 *  cola de transmisión.
 */
 void sendCmd( char* buffer, uint8_t length )
 {
     BaseType_t xStatus;
     const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
     for ( uint8_t i=0; i<length; i++ ) {
         xStatus = xQueueSendToBack( 
            uartTxQueue,    // Handle de la cola a escribir
            &buffer[i],     // Puntero a la información a escribir
            xTicksToWait    // Máxima cantidad de tiempo que la tarea permanecerá bloqueada hasta que la cola se encuentre disponible
            );
        
        if ( xStatus != pdPASS ) {
            // Escritura fallida, cola completa
            i--;
        }
     }
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
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
    for ( ;; ) {
        xStatus = xQueueReceive( uartRxQueue, &cRx, xTicksToWait );
        
        if ( xStatus == pdPASS ) {
            // Lectura exitosa, verificación de caracter final
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
        } else {
            // Lectura fallida, la cola se encuentra vacía
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
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );

    for (;;) {
        xStatus = xQueueReceive(
            uartTxQueue,    // Handle de la cola a leer
            &cTx,           // Puntero a la estructura que guardará la lectura
            xTicksToWait    // Máxima cantidad de tiempo que la tarea permanecerá bloqueada hasta que la cola se encuentre disponible
            );
        
        if ( xStatus == pdPASS ) {
            // Lectura exitosa, enviar dato
            uartWriteByte( UART_USB, cTx );
        } else {
            // Lectura fallida, la cola se encuentra vacía
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

int main(void)
{
    /* Inicialización de la placa */
    boardConfig();

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

    // Verificación de colas creadas con éxito
    if ( (uartRxQueue != NULL) && (uartTxQueue != NULL) ) {
        xTaskCreate(
            uartRxTask,                 // Funcion de la tarea a ejecutar
            (const char *)"uartRxTask", // Nombre de la tarea como String amigable para el usuario
            configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
            NULL,                       // Parametros de tarea
            tskIDLE_PRIORITY+1,         // Prioridad de la tarea
            NULL                        // Puntero a la tarea creada en el sistema
        );

        xTaskCreate( uartTxTask, (const char *)"uartTxTask",
            configMINIMAL_STACK_SIZE*2, NULL,
            tskIDLE_PRIORITY+1, NULL );
        
    } else {
        // TO DO: Warning con LED
    }

    // Inicialización de Scheduler
    vTaskStartScheduler();

    for ( ;; );

    return 0;
}