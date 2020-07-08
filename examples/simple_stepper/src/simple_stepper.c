/*! \file simple_stepper.c
    \brief Ejemplo sencillo de la implementación del control de motores paso a paso en el proyecto.
    \author Gonzalo G. Fernández
    \version 1.0
    \date 2020

    Detalle.
*/

/* Utilidades includes */
#include <string.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

/*! \def stepperAPP_NUM
    \brief Cantidad de motores en la aplicación
*/
#define stepperAPP_NUM  1

/*! \def stepperTIMER_PERIOD
    \brief Periodo de pasos de los motores (velocidad del motor)
*/
#define stepperTIMER_PERIOD 2

/*! \def stepperDIR_NEGATIVE
    \brief Dirección negativa del motor (depende de conexión del driver)
*/
#define stepperDIR_NEGATIVE 0

/*! \def stepperDIR_POSITIVE
    \brief Dirección negativa del motor (depende de conexión del driver)
*/
#define stepperDIR_POSITIVE 1

/*! \var typedef char StepperDir_t
    \brief Definición de tipo para dirección de motores stepper.
*/
typedef char StepperDir_t;

/*! \var typedef struct xStepperData StepperData_t
    \brief Estructura de datos con información del stepper 
*/
typedef struct xStepperData {
    /* Estado actual de entradas al driver */
    char cDriverState;
    /* Cantidad de pasos pendientes */
    uint32_t ulPendingSteps;
    /* Dirección en que se deben realizar los
    pasos pendientes */
    StepperDir_t xDir;
    /* Posición absoluta del motor */
    int32_t lAbsPosition;
} StepperData_t;

/*! \var StepperData_t xStepperDataID[stepperAPP_NUM]
    \brief Instanciación de información de motores 
*/
StepperData_t xStepperDataID[stepperAPP_NUM];

/*! \fn void vStepperDriverUpdate( uint8_t cState )
    \brief Escritura en driver de motor stepper dado un determinado estado.
    \param cState Estado a escribir en el driver.
*/
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

/*! \fn void prvStepperTimerCallback( TimerHandle_t xStepperTimer )
    \brief Función de callback que ejecutará cada timer al entrar en estado Running.
*/
static void prvStepperTimerCallback( TimerHandle_t xStepperTimer )
{
    /* Estructura de datos a obtener del timer ID */
    StepperData_t *xStepperDataID;
    xStepperDataID = ( StepperData_t * ) pvTimerGetTimerID( xStepperTimer );
    /* Verificación de existencia de pasos pendientes */
    if ( xStepperDataID->ulPendingSteps == 0 ) {
        /* Detener timer si no existen pasos pendientes */
        xTimerStop( xStepperTimer, 0 );
        return;
    }
    /* Cálculo de nuevo estado del driver según dirección */
    if ( xStepperDataID->xDir == stepperDIR_NEGATIVE ) {
        if ( xStepperDataID->cDriverState == 0 ) {
            xStepperDataID->cDriverState = 7;
        } else {
            xStepperDataID->cDriverState--;
        }
    } else if ( xStepperDataID->xDir == stepperDIR_POSITIVE ) {
        if ( xStepperDataID->cDriverState == 7 ) {
            xStepperDataID->cDriverState = 0;
        } else {
            xStepperDataID->cDriverState++;
        }
    }
    /* Actualización del driver */
    vStepperDriverUpdate( xStepperDataID->cDriverState );
    /* Decremento de pasos pendientes a realizar */
    xStepperDataID->ulPendingSteps--;
    /* Actualización del timer ID */
    vTimerSetTimerID( xStepperTimer, ( void * ) xStepperDataID );
}

uint8_t main( void )
{
    /* Inicialización de la placa */
    boardConfig();

    /* Inicialización de salidas a driver del stepper */
    gpioInit( GPIO0, GPIO_OUTPUT );
    gpioInit( GPIO1, GPIO_OUTPUT );
    gpioInit( GPIO2, GPIO_OUTPUT );
    gpioInit( GPIO3, GPIO_OUTPUT );

    /* Inicialización de información del stepper */
    xStepperDataID[0].ulPendingSteps = 5000;
    xStepperDataID[0].xDir = stepperDIR_POSITIVE;
    xStepperDataID[0].cDriverState = 0;
    xStepperDataID[0].lAbsPosition = 0;

    /* Handle de los timers */
    TimerHandle_t xStepperTimer[stepperAPP_NUM];
    BaseType_t xStepperTimerStarted[stepperAPP_NUM];

    //char pcAuxTimerName[15];
    //strcpy( pcAuxTimerName, "StepperTimer" );
    for (uint8_t i=0; i<stepperAPP_NUM; i++) {
        //strcat( pcAuxTimerName, "0" + i );
        /* Creación de los software timers */
        xStepperTimer[i] = xTimerCreate(
            /* Nombre descriptivo del timer */
            (const char *) "StepperTimer",
            /* Periodo del timer especificado en ticks */
            pdMS_TO_TICKS( stepperTIMER_PERIOD ),
            /* pdTRUE para timer tipo auto-reload y pdFALSE para tipo one-shoot */
            pdTRUE,
            /* Valor de ID del timer */
            ( void * ) &xStepperDataID[i],
            /* Función de callback del timer */
            prvStepperTimerCallback
        );

        if ( xStepperTimer[i] == NULL ) {
            /* Error al crear timer */
            return 1;
        } else {
            xTimerStart( xStepperTimer[i], 0 );
        }
    }

    vTaskStartScheduler();

    for ( ;; );
    return 0;
}
