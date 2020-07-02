/*
 *
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "sapi.h"
#include "uart.h"

uint8_t main( void )
{
    /* Inicialización de la placa */
    boardConfig();

    // Inicialización de UART
    if ( uartAppInit() ) {
        // TO DO: Warning con LED
    }

    // Inicialización de Scheduler
    vTaskStartScheduler();

    for ( ;; );

    return 0;
}