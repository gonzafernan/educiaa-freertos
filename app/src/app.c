/*
 *
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"

#include "sapi.h"
#include "uart.h"

#define AUTO_RELOAD_TIMER_PERIOD    pdMS_TO_TICKS( 1000 )

static void prvAutoReloadTimerCallback( TimerHandle_t xTimer )
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
    char cTick = 'T';
    xStatus = xQueueSendToBack( uartTxQueue, &cTick, xTicksToWait );
}

uint8_t main( void )
{
    /* Inicialización de la placa */
    boardConfig();

    TimerHandle_t xAutoReloadTimer;
    BaseType_t xTimerStarted;

    xAutoReloadTimer = xTimerCreate(
        /* Nombre descriptivo del timer */
        "AutoReload",
        /* Periodo del timer especificado en ticks */
        AUTO_RELOAD_TIMER_PERIOD,
        /* pdTRUE para timer tipo auto-reload y pdFALSE para tipo one-shoot */
        pdTRUE,
        /* Valor de ID del timer */
        NULL,
        /* Función de callback del timer */
        prvAutoReloadTimerCallback
    );

    if ( xAutoReloadTimer != NULL ) {
        // Inicialización de timer
        xTimerStarted = xTimerStart(
            /* Handle del timer a iniciar o resetear */
            xAutoReloadTimer,
            /* Máximo tiempo de espera en estado bloqueado a que haya espacio en la cola de timers */
            0
        );
        // Chequeo de falla si cola de timers esta completa
        if ( xTimerStarted != pdPASS ) {
            // TO DO: Warning con LED
        }
    }

    // Inicialización de UART
    if ( uartAppInit() ) {
        // TO DO: Warning con LED
    }

    // Inicialización de Scheduler
    vTaskStartScheduler();

    for ( ;; );

    return 0;
}