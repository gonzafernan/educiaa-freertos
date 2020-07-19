/*! \file app.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

/* Utilidades includes */
#include <string.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOSPriorities.h"
#include "task.h"
#include "queue.h"

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

/* Aplicación includes */
#include "uart.h"
#include "encoder.h"
#include "stepper.h"
#include "servo.h"

/*! \def appQUEUE_MSG_LENGTH
	\brief Longitud de cola de mensajes recibidos.
*/
#define appQUEUE_MSG_LENGTH	50

/*! \var TaskHandle_t xAppSyncTaskHandle
	\brief Handle de la tarea que sincroniza mensajes.
*/
TaskHandle_t xAppSyncTaskHandle = NULL;

/*! \var QueueHandle_t xMsgQueue
    \brief Cola de mensajes recibidos.
*/
QueueHandle_t xMsgQueue;

/*! \fn vErrorNotifHandling( uint32_t ulNotifError )
	\brief Gestió de errores obtenidos por notificación de tarea.
*/
void vErrorNotifHandling( uint32_t ulNotifError )
{
	if ( ulNotifError > 0x00 ) {
		/* Error en stepper ID */
		if ( ulNotifError & (1 << stepperERROR_NOTIF_ID ) ) {
			vUartSendMsg( "AST:ERR:STPID" );
		}
		/* Error en stepper DIR */
		if ( ulNotifError & (1 << stepperERROR_NOTIF_DIR ) ) {
			vUartSendMsg( "AST:ERR:STPDIR" );
		}
		/* Error en stepper VEL */
		if ( ulNotifError & (1 << stepperERROR_NOTIF_VEL ) ) {
			vUartSendMsg( "AST:ERR:STPVEL" );
		}
		/* Error en stepper ANG */
		if ( ulNotifError & (1 << stepperERROR_NOTIF_ANG ) ) {
			vUartSendMsg( "AST:ERR:STPANG" );
		}
		/* Error en stepper ID */
		if ( ulNotifError & (1 << servoERROR_NOTIF_ANG ) ) {
			vUartSendMsg( "AST:ERR:SRVANG" );
		}
	}
}

/*! \fn vAppSyncTask( void *pvParameters )
	\brief Tarea de sincronización de mensajes.
*/
void vAppSyncTask( void *pvParameters )
{
    /* Puntero al mensaje leído */
    char *pcMsgReceived;
    /* Valor de notificaciones pendientes */
    uint32_t ulNotifError;
    /* Mensaje de error */
    char *pcErrorMsg = "Error en mensaje previo";

    for ( ;; ) {
        xQueueReceive(
            /* Handle de la cola a leer */
            xMsgQueue,
            /* Puntero a la memoria donde guardar lectura */
            &pcMsgReceived,
            /* Máximo tiempo que la tarea puede estar bloqueada
            esperando que haya información a leer */
            portMAX_DELAY
        );

        /* Verificación de inicio de trama */
        if ( pcMsgReceived[0] != ':' ) {
        	/* Mensaje de comando inválido */
        	strcat( pcMsgReceived, " - error" );
        	vUartSendMsg( pcMsgReceived );
        	continue;
        }
        /* Consigna a motores stepper */
        if ( pcMsgReceived[1] == 'X' ) {
        	/* Escribir mensaje en cola de consignas */
        	vServoSendMsg( pcMsgReceived );
        }
        /* Consigna a motor servo */
        if ( pcMsgReceived[1] == 'S' ) {
			/* Escribir mensaje en cola de consignas */
			vStepperSendMsg( pcMsgReceived );
		}

        /* Verificación de notificación de error */
        ulNotifError = ulTaskNotifyTake( pdTRUE, 0 );
        vErrorNotifHandling( ulNotifError );
    }
}

/*! \fn void vLedBlinkTask( void *pvParameters )
	\brief Tarea con blink de LED para visual de aplicación
	funcionando
*/
void vLedBlinkTask( void *pvParameters )
{
	for ( ;; ) {
		/* Indicador en LED1 */
		gpioToggle( LED1 );
		/* Parpadeo de medio segundo */
		vTaskDelay( pdMS_TO_TICKS( 500 ) );
	}
}

int main( void )
{
    /* Inicialización de la placa */
    boardConfig();

    /* Flags de estado de los diferentes módulos */
    BaseType_t xUartStatus, xEncoderStatus, xStepperStatus;
    /* Inicialización de UART */
    xUartStatus = xUartInit();
    /* Inicialización de encoder rotativo */
    xEncoderStatus = xEncoderInit();
    /* Inicialización de motor stepper */
    xStepperStatus = xStepperInit();
    /* Inicialización de servo motor */
    xServoInit();

    /* Creación de cola de mensajes recibidos */
    xMsgQueue = xQueueCreate( appQUEUE_MSG_LENGTH, sizeof( char * ) );

    /* Creación de tarea de control de flujo de trabajo del programa */
    BaseType_t xStatus;
    xStatus = xTaskCreate(
        /* Puntero a la función que implementa la tarea */
        vAppSyncTask,
        /* Nombre de la tarea amigable para el usuario */
        ( const char * ) "AppSyncTask",
        /* Tamaño de stack de la tarea */
        configMINIMAL_STACK_SIZE*2,
        /* Parámetros de la tarea */
        NULL,
        /* Prioridad de la tarea */
		priorityAppSyncTask,
        /* Handle de la tarea creada */
        &xAppSyncTaskHandle
    );

    /* Tarea con blink de LED para visual de aplicación funcionando */
    xStatus = xTaskCreate( vLedBlinkTask, ( const char * ) "LedBlinkTask",
    	configMINIMAL_STACK_SIZE, NULL, priorityLedBlinkTask, NULL );

    /* Inicialización de Scheduler */
    vTaskStartScheduler();

    for ( ;; );

    return 0;
}
