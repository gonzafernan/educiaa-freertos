/*
 *
 */

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "timers.h"

#include "sapi.h"

#define stepperNUM   3

uint8_t iter = 0;

void vStepperDriverUpdate( uint8_t cState )
{
    switch ( cState ) {
        case 0:
            gpioWrite(GPIO0, ON);
            gpioWrite(GPIO1, ON);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, OFF);
            break;

        case 1:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, ON);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, OFF);
            break;

        case 2:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, ON);
            gpioWrite(GPIO2, ON);
            gpioWrite(GPIO3, OFF);
            break;

        case 3:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, ON);
            gpioWrite(GPIO3, OFF);
            break;

        case 4:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, ON);
            gpioWrite(GPIO3, ON);
            break;

        case 5:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, ON);
            break;

        case 6:
            gpioWrite(GPIO0, ON);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, ON);
            break;

        case 7:
            gpioWrite(GPIO0, ON);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, OFF);
            break;
    }
}

void prvStepperTimerCallback( TimerHandle_t xStepperTimer )
{
    vStepperDriverUpdate( iter );
    if ( iter == 7 ) {
        iter = 0;
    } else {
        iter++;
    }
}

uint8_t main( void )
{
    /* Inicialización de la placa */
    boardConfig();

    /* Inicialización de salidas a driver del stepper */
    gpioInit(GPIO0, GPIO_OUTPUT);
    gpioInit(GPIO1, GPIO_OUTPUT);
    gpioInit(GPIO2, GPIO_OUTPUT);
    gpioInit(GPIO3, GPIO_OUTPUT);

    /* Handle de los timers */
    TimerHandle_t xStepperTimer;
    BaseType_t xStepperTimerStarted;

    /* Creación de los software timers */
    xStepperTimer = xTimerCreate(
        /* Nombre descriptivo del timer */
        (const char *)"StepperTimer",
        /* Periodo del timer especificado en ticks */
        pdMS_TO_TICKS( 1 ),
        /* pdTRUE para timer tipo auto-reload y pdFALSE para tipo one-shoot */
        pdTRUE,
        /* Valor de ID del timer */
        NULL,
        /* Función de callback del timer */
        prvStepperTimerCallback
    );

    if ( xStepperTimer != NULL ) {
        xStepperTimerStarted = xTimerStart( xStepperTimer, 0 );
        if ( xStepperTimerStarted == pdPASS ) {
            vTaskStartScheduler();
        }
    }

    for ( ;; );
    return 0;
}
