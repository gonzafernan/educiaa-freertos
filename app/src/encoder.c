/*! \file encoder.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Para configurar cualquiera de los pines, es necesario conocer los
    siguientes datos:
    - De SCU: SCU_PORT, SCU_PIN, SCU_FUNC
    - De GPIO: GPIO_PORT, GPIO_PIN

    GPIO0	P6_1	GPIO3[0]
	GPIO1	P6_4	GPIO3[3]
	GPIO2	P6_5	GPIO3[4]
*/

/* Utilidades includes */
#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "FreeRTOSPriorities.h"

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"
#include "chip.h"

/* Aplicación includes */
#include "encoder.h"
#include "uart.h"
#include "stepper.h"
#include "servo.h"
#include "display_lcd.h"

/*! \var SemaphoreHandle_t xEncoderPositivePulseSemaphore
	\brief Semáforo que guarda la cantidad de pulsos positivos
	generados por el encoder en pin clock.
*/
SemaphoreHandle_t xEncoderPositivePulseSemaphore;

/*! \var SemaphoreHandle_t xEncoderNegativePulseSemaphore
	\brief Semáforo que guarda la cantidad de pulsos negativos
	generados por el encoder en pin clock.
*/
SemaphoreHandle_t xEncoderNegativePulseSemaphore;

/*! \var QueueHandle_t xEncoderChoiceMailbox
	\brief Mailbox con la selección actual de motor mediante
	pulsador de encoder.
*/
QueueHandle_t xEncoderChoiceMailbox;

/*! \fn void vDeferredHandlingFunction( void* pvParameter1, uint32_t ulParameter2 )
	\brief Función a ejecutar como procesamiento diferido a tarea RTOS daemon
	desde encoder (ante el pulsador).
	\param pvParameter1 Primer parámetro de la función en forma void* para apuntar
	a estructura.
	\param ulParameter2 Segundo parámetro de la función en forma de uint32_t.
*/
static void vDeferredHandlingFunction( void* pvParameter1, uint32_t ulParameter2 )
{
	/* Valor de selección del encoder */
	uint8_t cValue;
	/* Lectura del valor actual en mailbox */
	xQueuePeek( xEncoderChoiceMailbox, &cValue, 0 );
	/* Incremento en la selección */
	cValue++;
	/* Vuelta a cero por Overflow del menu */
	if ( cValue > stepperAPP_NUM + 1 ) {
		cValue = 0;
	}
	/* Actualización del valor en mailbox */
	xQueueOverwrite( xEncoderChoiceMailbox, &cValue );

	/* Actualización de selección en display
	 * (implementación en display_lcd.c) */
	vUpdateSelection( cValue );
	vUartSendMsg("SW");
}

/*! \fn void PININT1_IRQ_HANDLER( void )
	\brief Handler interrupt from GPIO pin or GPIO pin mapped to PININT
*/
void vEncoderCLK_IRQ_HANDLER( void )
{
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if ( gpioRead( encoderPIN_DT ) ) {
		/* Agregar pulso positivo al semáforo contador */
		xSemaphoreGiveFromISR( xEncoderPositivePulseSemaphore,
			&xHigherPriorityTaskWoken );
	} else {
		/* Agregar pulso positivo al semáforo contador */
		xSemaphoreGiveFromISR( xEncoderNegativePulseSemaphore,
			&xHigherPriorityTaskWoken );
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*! \fn void PININT2_IRQ_HANDLER( void )
	\brief Handler interrupt from GPIO pin or GPIO pin mapped to PININT
*/
//void vEncoderSW_IRQ_HANDLER( void )
void PININT_IRQ_HANDLER( void )
{
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT_INDEX ) );

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* Procesamiento diferido al RTOS daemon */
	xTimerPendFunctionCallFromISR(
			/* Puntero a la función a ejecutar en la tarea daemon */
			vDeferredHandlingFunction,
			/* Primer parámetro de la función en forma void* para apuntar a estructura */
			NULL,
			/* Segundo parámetro de la función en forma de uint32_t */
			0,
			/* Parámetro para chequear por tarea de mayor prioridad */
			&xHigherPriorityTaskWoken
			);

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*! \fn void vEncoderTask( void *pvParameters )
	\brief Tarea de procesamiento de información de encoder.
*/
void vEncoderTask( void *pvParameters )
{
	/* Valor de selección inicial del encoder */
	uint8_t cValue = 0;
	xQueueOverwrite( xEncoderChoiceMailbox, &cValue );

	/* Creación de set de semáforos */
	static QueueSetHandle_t xEncoderSemaphoreSet;
	xEncoderSemaphoreSet = xQueueCreateSet( 2 * encoderMAX_CLK_PULSES );
	/* Agregar semáforos al set */
	xQueueAddToSet( xEncoderPositivePulseSemaphore, xEncoderSemaphoreSet );
	xQueueAddToSet( xEncoderNegativePulseSemaphore, xEncoderSemaphoreSet );
	/* Semáforo con información */
	SemaphoreHandle_t xReceivedSemaphore;

	/* Puntero al mensaje a enviar */
	char *pcMsgToSend = ( char * ) pvPortMalloc( 10 * sizeof( char ) );
	/* Caracter auxiliar */
	char cAuxChar[2] = "0";

	for ( ;; ) {
		/* Lectura de selección de motor en mailbox */
		xQueuePeek( xEncoderChoiceMailbox, &cValue, portMAX_DELAY );
		/* Inicio de mensaje */
		if ( cValue < stepperAPP_NUM ) {
			/* Consigna a motor paso a paso */
			strcpy( pcMsgToSend, ":S" );
			cAuxChar[0] = cValue + '0';
			strcat( pcMsgToSend, cAuxChar );
		} else if ( cValue == stepperAPP_NUM ) {
			/* Consigna a servomotor */
			strcpy( pcMsgToSend, ":X" );
		} else {
			/* Excepción, valor no válido (no debería suceder) */
			continue;
		}

		/* Lectura de handle del set */
		xReceivedSemaphore = ( SemaphoreHandle_t ) xQueueSelectFromSet( xEncoderSemaphoreSet, portMAX_DELAY );
		/* Obtener que semáforo tiene información y realizar take */
		if ( xReceivedSemaphore == xEncoderPositivePulseSemaphore ) {
			xSemaphoreTake( xEncoderPositivePulseSemaphore, portMAX_DELAY );
			if ( cValue < stepperAPP_NUM ) {
				strcat( pcMsgToSend, "D\0" );
				cAuxChar[0] = (char)stepperDIR_POSITIVE + '0';
				strcat( pcMsgToSend, cAuxChar );
				/* Ángulo a setear */
				strcat( pcMsgToSend, "A015" );
				/* Enviar mensaje a cola de consignas */
				vStepperSendMsg( pcMsgToSend );
			} else {
				uint8_t cPositionValue;
				xQueuePeek( xServoPositionMailbox, &cPositionValue, portMAX_DELAY );
				if ( cPositionValue >= 180 ) {
					continue;
				} else {
					cPositionValue += servoVALUE_INCRMENT;
					char pcAuxValue[4];
					sprintf( pcAuxValue, "%d", cPositionValue );
					strcat( pcMsgToSend, pcAuxValue );
					vServoSendMsg( pcMsgToSend );
				}
			}
		} else if ( xReceivedSemaphore == xEncoderNegativePulseSemaphore ) {
			xSemaphoreTake( xEncoderNegativePulseSemaphore, portMAX_DELAY );

			if ( cValue < stepperAPP_NUM ) {
				strcat( pcMsgToSend, "D\0" );
				cAuxChar[0] = (char)stepperDIR_NEGATIVE + '0';
				strcat( pcMsgToSend, cAuxChar );
				/* Ángulo a setear */
				strcat( pcMsgToSend, "A015" );
				/* Enviar mensaje a cola de consignas */
				vStepperSendMsg( pcMsgToSend );
			} else {
				uint8_t cPositionValue;
				xQueuePeek( xServoPositionMailbox, &cPositionValue, portMAX_DELAY );
				if ( cPositionValue <= 0 ) {
					continue;
				} else {
					cPositionValue -= servoVALUE_INCRMENT;
					char pcAuxValue[4];
					sprintf( pcAuxValue, "%d", cPositionValue );
					strcat( pcMsgToSend, pcAuxValue );
					vServoSendMsg( pcMsgToSend );
				}
			}
		}

		vUartSendMsg( pcMsgToSend );
	}
}

/*! \fn BaseType_t xEncoderInit( void )
    \brief Inicialización de encoder rotativo de la aplicación.
*/
BaseType_t xEncoderInit( void )
{
	/* Configuración de pulsador como entrada */
	gpioConfig( GPIO0, GPIO_INPUT_PULLUP );

	Chip_SCU_GPIOIntPinSel(
			/* GPIO PINTSEL interrupt, should be: 0 to 7 */
			PININT_INDEX,
			/* GPIO port number interrupt, should be: 0 to 7 */
			GPIO0_GPIO_PORT,
			/* GPIO pin number Interrupt , should be: 0 to 31 */
			GPIO0_GPIO_PIN
			);

	Chip_PININT_ClearIntStatus(
			/* The base address of Pin interrupt block */
			LPC_GPIO_PIN_INT,
			/* Pin interrupts to clear (ORed value of PININTCH*) */
			PININTCH( PININT_INDEX )
			);

	Chip_PININT_SetPinModeEdge(
			/* The base address of Pin interrupt block */
			LPC_GPIO_PIN_INT,
			/* Pin interrupts to clear (ORed value of PININTCH*) */
			PININTCH( PININT_INDEX )
			);

	Chip_PININT_EnableIntHigh(
			/* The base address of Pin interrupt block */
			LPC_GPIO_PIN_INT,
			/* Pin interrupts to clear (ORed value of PININTCH*) */
			PININTCH( PININT_INDEX ) );

	NVIC_ClearPendingIRQ( PININT_NVIC_NAME );
	NVIC_EnableIRQ( PININT_NVIC_NAME );
	NVIC_SetPriority( PININT_NVIC_NAME, 255 );

	/* Configuracion de pines de la EDU-CIAA-NXP como entrada con pull-up */
//	gpioConfig( encoderPIN_SW, GPIO_INPUT_PULLUP );
//	gpioConfig( encoderPIN_CLK, GPIO_INPUT_PULLUP );
//	gpioConfig( encoderPIN_DT, GPIO_INPUT_PULLUP );

	/*
	* Select irq channel to handle a GPIO interrupt, using its port and pin to specify it
	* From EduCiaa pin out spec: GPIO1[9] -> port 1 and pin 9
	*/
//	Chip_SCU_GPIOIntPinSel( PININT1_INDEX, GPIO1_GPIO_PORT, GPIO1_GPIO_PIN );
//	/* Clear actual configured interrupt status */
//	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
//	/* Set edge interrupt mode */
//	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
//	/* Enable high edge gpio interrupt */
//	Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
//	/* Clear pending irq channel interrupts */
//	NVIC_ClearPendingIRQ( PIN_INT0_IRQn + PININT1_INDEX );
//	/* Enable irqChannel interrupt */
//	NVIC_EnableIRQ( PIN_INT0_IRQn + PININT1_INDEX );
//	/* Seteo del nivel de prioridad de la interrupción 0 */
//	NVIC_SetPriority( PININT1_NVIC_NAME, 255 );

	/*
	* Select irq channel to handle a GPIO interrupt, using its port and pin to specify it
	* From EduCiaa pin out spec: GPIO1[9] -> port 1 and pin 9
	*/
//	Chip_SCU_GPIOIntPinSel( PININT2_INDEX, GPIO2_GPIO_PORT, GPIO2_GPIO_PIN );
//	/* Clear actual configured interrupt status */
//	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );
//	/* Set edge interrupt mode */
//	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );
//	/* Enable high edge gpio interrupt */
//	Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );
//	/* Clear pending irq channel interrupts */
//	NVIC_ClearPendingIRQ( PIN_INT0_IRQn + PININT2_INDEX );
//	/* Enable irqChannel interrupt */
//	NVIC_EnableIRQ( PIN_INT0_IRQn + PININT2_INDEX );
//	/* Seteo del nivel de prioridad de la interrupción 0 */
//	NVIC_SetPriority( PININT2_NVIC_NAME, 255 );

	/* Creación de mailbox con selección de motor */
	xEncoderChoiceMailbox = xQueueCreate( 1, sizeof( uint8_t ) );
	/* Verificación de mailbox creado con éxito */
	configASSERT( xEncoderChoiceMailbox != NULL );

	/* Creación de semáforo contador para pulsos positivos de clock */
	xEncoderPositivePulseSemaphore = xSemaphoreCreateCounting(
		/* Cantidad máxima de elementos */
		encoderMAX_CLK_PULSES,
		/* Cuenta inicial del semáforo */
		0
	);
	/* Creación de semáforo contador para pulsos negativos de clock */
	xEncoderNegativePulseSemaphore = xSemaphoreCreateCounting(
		/* Cantidad máxima de elementos */
		encoderMAX_CLK_PULSES,
		/* Cuenta inicial del semáforo */
		0
	);

	/* Creación de tarea de procesamiento de información de encoder */
	BaseType_t xStatus = pdPASS;
	xStatus = xTaskCreate(
		/* Puntero a la función que implementa la tarea */
		vEncoderTask,
		/* Nombre de la tarea amigable para el usuario */
		( const char * ) "EncoderTask",
		/* Tamaño de stack de la tarea */
		configMINIMAL_STACK_SIZE*2,
		/* Parámetros de la tarea */
		NULL,
		/* Prioridad de la tarea */
		priorityEncoderTask,
		/* Handle de la tarea creada */
		NULL
	);

	return xStatus;
}
