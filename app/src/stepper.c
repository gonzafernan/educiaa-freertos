/*! \file stepper.c
    \brief Tarea, timers, y funciones asociado al control
    de los motores paso a paso.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020
*/

/* Utilidades includes */
#include <stdlib.h>
#include <string.h>

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

/* Aplicación includes */
#include "stepper.h"
#include "driver_uln2003.h"
#include "uart.h"

/* FreeRTOS includes */
#include "FreeRTOSPriorities.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/*! \def stepperANGLE_TO_STEPS( X )
    \brief Macro para convertir ángulos en cantidad de pasos del motor.
*/
#define stepperANGLE_TO_STEPS( x )  4096*( x )/360

/*! \def stepperSTEPS_TO_ANGLE( X )
    \brief Macro para convertir pasos del motor a ángulo.
*/
#define stepperSTEPS_TO_ANGLE( x )  360*( x )/4096

/*! \var typedef struct xStepperData StepperData_t
    \brief Estructura de datos con información del stepper.
*/
typedef struct xStepperData {
	/* Conjunto de entradas al driver correspondiente */
	DriverIn_t pxDriverInput[4];
    /* Estado actual de entradas al driver */
    char cDriverState;
    /* Cantidad de pasos pendientes */
    uint32_t ulPendingSteps;
    /* Dirección en que se deben realizar los
    pasos pendientes */
    StepperDir_t xDir;
    /* Bit asociado en el grupo de eventos (igual a ID) */
    uint8_t cEventGroupBit;
    /* LED asociado al motor como indicador visual */
    gpioMap_t xLed;
} StepperData_t;

/*! \var TaskHandle_t xStepperControlTaskHandle
	\brief Handle de la tarea que controla el flujo de trabajo
	de los motores.
*/
TaskHandle_t xStepperControlTaskHandle = NULL;

/*! \var TaskHandle_t xAppSyncTaskHandle
	\brief Handle de la tarea que sincroniza mensajes.
*/
extern TaskHandle_t xAppSyncTaskHandle;

/*! \var TimerHandle_t xStepperTimer[stepperAPP_NUM]
    \brief Array de handles de los timers asociados a los motores.
*/
TimerHandle_t xStepperTimer[stepperAPP_NUM];

/*! \var StepperData_t xStepperDataID[stepperAPP_NUM]
    \brief Instanciación de información de motores 
*/
StepperData_t xStepperDataID[stepperAPP_NUM];

/*! \var QueueHandle_t xStepperSetPointQueue
    \brief Cola de consignas recibidas a ejecutar.
*/
QueueHandle_t xStepperSetPointQueue;

/*! \var EventGroupHandle_t xStepperEventGroup
	\brief Grupo de eventos para detectar que todos los
	motores detuvieron su movimiento (final de consigna).
*/
EventGroupHandle_t xStepperEventGroup;

/*! \fn uint32_t ulStepperGetAngle( uint8_t ucStepperIndex )
	\brief Obtener ángulo pendiente de motor paso a paso.
	\param ucStepperIndex Índice del motor paso a paso.
	\return Pasos pendientes del motor en forma de ángulo.
*/
uint32_t ulStepperGetAngle( uint8_t ucStepperIndex )
{
	/* Estructura de datos a obtener del timer ID */
	StepperData_t* xStepperDataID;
	xStepperDataID = ( StepperData_t * ) pvTimerGetTimerID( xStepperTimer[ucStepperIndex] );

	/* Devolver pasos pendientes en forma de ángulo */
	return xStepperDataID->ulPendingSteps * 360/4096;
}

/*! \fn void vStepperSendMsg( char *pcMsg )
	\brief Enviar consigna a cola de consignas pendientes.
	\param pcMsg String con consigna a enviar.
*/
void vStepperSendMsg( char *pcMsg)
{
	/* Escribir mensaje en cola de transmisión */
	xQueueSendToBack(
		/* Handle de la cola a escribir */
		xStepperSetPointQueue,
		/* Puntero al dato a escribir */
		&pcMsg,
		/* Máximo tiempo a esperar una escritura */
		portMAX_DELAY
	);
}

/*! \fn void vStepperRelativeSetPoint( TimerHandle_t xStepperTimer, int32_t lRelativeSetPoint )
    \brief Setear una nueva consigna en motor stepper relativa a la posición actual.
    \param xStepperTimer Handle del timer asociado al motor que se desea asignar la nueva consigna.
    \param ulRelativeSetPoint Cantidad de pasos a realizar.
    \param xStepperDir Dirección de los pasos a realizar.
*/
BaseType_t xStepperRelativeSetPoint( TimerHandle_t xStepperTimer, uint32_t ulRelativeSetPoint, StepperDir_t xStepperDir )
{
    /* Estructura de datos a obtener del timer ID */
    StepperData_t *xStepperDataID;
    xStepperDataID = ( StepperData_t * ) pvTimerGetTimerID( xStepperTimer );

    /* Seteo de consigna como pasos pendientes y dirección */
    xStepperDataID->ulPendingSteps = ulRelativeSetPoint;
    xStepperDataID->xDir = xStepperDir;
    /* Inicialización de timer */
    return xTimerStart( xStepperTimer, portMAX_DELAY );
}

/*! \fn void prvStepperTimerCallback( TimerHandle_t xStepperTimer )
    \brief Función de callback que ejecutará cada timer al entrar en estado Running.
*/
static void prvStepperTimerCallback( TimerHandle_t xStepperTimer )
{
    /* Estructura de datos a obtener del timer ID */
    StepperData_t *xStepperDataID;
    xStepperDataID = ( StepperData_t * ) pvTimerGetTimerID( xStepperTimer );

    /* LED indicador visual ON */
    gpioWrite( xStepperDataID->xLed, ON );

    /* Verificación de existencia de pasos pendientes */
    if ( xStepperDataID->ulPendingSteps == 0 ) {
    	/* LED indicador visual OFF */
		gpioWrite( xStepperDataID->xLed, OFF );

    	/* Detener timer si no existen pasos pendientes */
        xTimerStop( xStepperTimer, 0 );
        /* Setear bit de evento asociado al motor */
        xEventGroupSetBits(
        	/* Handle del grupo de eventos */
			xStepperEventGroup,
			/* Bits a setear */
			( 1 << xStepperDataID->cEventGroupBit )
        );
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
    vDriverUpdate( xStepperDataID->pxDriverInput,
    	xStepperDataID->cDriverState );
    /* Decremento de pasos pendientes a realizar */
    xStepperDataID->ulPendingSteps--;
    /* Actualización del timer ID */
    vTimerSetTimerID( xStepperTimer, ( void * ) xStepperDataID );
}

/*! \fn void vStepperControlTask( void *pvParameters )
    \brief Tarea encargada del control en el flujo de trabajo de los motores stepper, gestionando consignas y todo procesamiento relacionado con ellos.
*/
void vStepperControlTask( void *pvParameters )
{
    /* Puntero a consignas recibidas */
    char *pcReceivedSetPoint;

    /* ID y velocidad del motor recibida */
    uint8_t cID, cVel;
    /* Dirección */
    StepperDir_t xDir;
    /* Ángulo a realizar */
    int32_t lAngle;
    /* Variable para gestión de errores en mensaje */
    uint8_t cErrorHandle = 0;

    for ( ;; ) {
    	/* Lectura de cola de consignas */
        xQueueReceive(
            /* Handle de la cola a leer */
            xStepperSetPointQueue,
            /* Elemento donde guardar información leída */
            &pcReceivedSetPoint,
            /* Máxima cantidad de tiempo a esperar por una lectura */
            portMAX_DELAY
        );

        for ( uint8_t i=0; i<stepperAPP_NUM; i++ ) {
        	/* Lectura de ID del motor a setear */
			cID = atoi( &pcReceivedSetPoint[i*8+2] );
			/* Verificación de ID válida */
			if ( ( cID < 0 ) || ( cID >= stepperAPP_NUM ) ) {
				/* Código error por ID errónea */
				cErrorHandle = stepperERROR_NOTIF_ID;
				continue;
			}
			/* Lectura de dirección del motor */
			if ( pcReceivedSetPoint[i*8+3] == 'D' ) {
				xDir = atoi( &pcReceivedSetPoint[i*8+4] );
				/* Verificación de valor de dirección */
				if ( ( xDir < 0 ) || ( xDir > 1 ) ) {
					/* Código error por DIR errónea */
					cErrorHandle = stepperERROR_NOTIF_DIR;
					continue;
				}
			} else if ( pcReceivedSetPoint[i*8+3] == 'V' ) {
	        	cVel = atoi( &pcReceivedSetPoint[4] );
	        	/* Verificación de valor de velocidad */
	        	if ( ( cVel < stepperTIMER_MIN_PERIOD ) ||
	        			( cVel > stepperTIMER_MAX_PERIOD ) ) {
	        		/* Código error por VEL errónea */
	        		cErrorHandle = stepperERROR_NOTIF_VEL;
					continue;
	        	}
	        	/* Cambio de periodo del timer */
				xTimerChangePeriod(
					/* Handle del timer a modificar */
					xStepperTimer[cID],
					/* Nuevo periodo en ticks */
					pdMS_TO_TICKS( cVel ),
					/* Máximo tiempo a esperar bloqueado */
					portMAX_DELAY
					);
				break;
	        } else {
	        	/* Error en dirección o velocidad */
	        	cErrorHandle = stepperERROR_NOTIF_DIR;
	        	continue;
	        }

			/* Lectura de ángulo */
			if ( pcReceivedSetPoint[i*8+5] == 'A' ) {
				lAngle = atoi( &pcReceivedSetPoint[i*8+6] );
				/* Verificación de ángulo positivo */
				if ( lAngle < 0 ) {
					/* Código error por ANG erróneo */
					cErrorHandle = stepperERROR_NOTIF_ANG;
					continue;
				}
				/* Seteo de consigna */
				lAngle = stepperANGLE_TO_STEPS( lAngle );
				xStepperRelativeSetPoint( xStepperTimer[cID], lAngle, xDir );
			} else {
				/* Código error por ANG erróneo */
				cErrorHandle = stepperERROR_NOTIF_ANG;
				continue;
			}
        }
        /* Error en consigna a motores */
        if ( cErrorHandle ) {
        	xTaskNotify(
				/* Handle de la tarea a notificar */
				xAppSyncTaskHandle,
				/* Valor dependiente de eNotifyAction */
				( 1 << cErrorHandle ),
				/* Tipo enumerate que define cómo actualizar el valor
				de notificación en la tarea que recibe */
				eSetBits
			);
			continue;
        }
		/* Enviar mensaje de inicio de consigna */
        vUartSendMsg( "SCT:BGN" );

        /* Esperar finalización de ejecución de consigna */
		xEventGroupWaitBits(
			/* Handle de evento a leer */
			xStepperEventGroup,
			/* Bits que se epera */
			( 1 << 0 ) | ( 1 << 1 ),
			/* Clear de los bits setados si se cumple condición */
			pdTRUE,
			/* Condición de AND (esperar todos los bits) */
			pdTRUE,
			/* Bloquearse indefinidamente */
			portMAX_DELAY
		);

		/* Enviar mensaje de finalización de consigna */
		vUartSendMsg( "SCT:END" );
    }
}

/*! \fn BaseType_t xStepperInit( void )
    \brief Inicialización de motores de la aplicación.
*/
BaseType_t xStepperInit( void )
{
    /* Creación de tarea de control de flujo de trabajo de los motores stepper */
    BaseType_t xStatus;
    xStatus = xTaskCreate(
        /* Puntero a la función que implementa la tarea */
        vStepperControlTask,
        /* Nombre de la tarea amigable para el usuario */
        ( const char * ) "StepperControlTask",
        /* Tamaño de stack de la tarea */
        configMINIMAL_STACK_SIZE*2,
        /* Parámetros de la tarea */
        NULL,
        /* Prioridad de la tarea */
		priorityStepperControlTask,
        /* Handle de la tarea creada */
        &xStepperControlTaskHandle
    );

    if ( xStatus == pdFAIL ) {
        return pdFAIL;
    }
    
    /* Creación de cola de consignas recibidas a ejecutar */
	xStepperSetPointQueue = xQueueCreate(
		/* Longitud máxima de la cola */
		stepperMAX_SETPOINT_QUEUE_LENGTH,
		/* Tamaño de elementos a guardar en cola */
		sizeof( char * )
	);
	/* Verificación de cola creada con éxito */
	configASSERT( xStepperSetPointQueue != NULL );

	/* Array de LED indicadores visuales */
	gpioMap_t xLedArray[3] = { LED1, LED2, LED3 };

	/* Creación de grupo de eventos para detectar final de consigna */
	xStepperEventGroup = xEventGroupCreate();

    char pcAuxTimerName[15];
    for (uint8_t i=0; i<stepperAPP_NUM; i++) {
        /* String identificadora para debug */
    	strcpy( pcAuxTimerName, "StepperTimer" );
        strcat( pcAuxTimerName, "0" + i );

        /* Inicialización de driver del stepper */
		vDriverInit( pxDriver[i], driverINPUT_NUM );

        /* Vinculación con drivers */
		for ( uint8_t j=0; j<driverINPUT_NUM; j++) {
			xStepperDataID[i].pxDriverInput[j] = pxDriver[i][j];
		}

		/* LED indicador asociado */
		xStepperDataID[i].xLed = xLedArray[i];

		/* Bit de grupo de evento */
		xStepperDataID[i].cEventGroupBit = i;

        /* Creación de los software timers */
        xStepperTimer[i] = xTimerCreate(
            /* Nombre descriptivo del timer */
            (const char *)pcAuxTimerName,
            /* Periodo del timer especificado en ticks */
            pdMS_TO_TICKS( stepperTIMER_PERIOD ),
            /* pdTRUE para timer tipo auto-reload y pdFALSE para tipo one-shoot */
            pdTRUE,
            /* Valor de ID del timer */
            ( void * ) &xStepperDataID[i],
            /* Función de callback del timer */
            prvStepperTimerCallback
        );

        /* Si saco este printf para tres motores no funciona,
         * para dos motores si (?????????????????) */
        printf("---");

        if ( xStepperTimer[i] == NULL ) {
            /* Error al crear timer */
            return pdFAIL;
        }
    }
    /* Inicialización exitosa */
    return pdTRUE;
}
