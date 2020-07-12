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

/* FreeRTOS includes */
#include "FreeRTOSPriorities.h"

/* Aplicación includes */
#include "encoder.h"
#include "stepper.h"

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
void vEncoderSW_IRQ_HANDLER( void )
{
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	/* Valor de selección del encoder */
	uint8_t cValue;
	/* Lectura del valor actual en mailbox */
	xQueuePeekFromISR( xEncoderChoiceMailbox, &cValue );
	//cValue++;
	if ( cValue == 0 ) {
		cValue = 1;
	} else if ( cValue == 1 ) {
		cValue = 0;
	}
	printf("%d\n", cValue);
	/* Actualización del valor en mailbox */
	xQueueOverwriteFromISR( xEncoderChoiceMailbox, &cValue, 0 );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*! \fn void vEncoderTask( void *pvParameters )
	\brief Tarea de procesamiento de información de encoder.
*/
void vEncoderTask( void *pvParameters )
{
	/* Creación de mailbox con selección de motor */
	xEncoderChoiceMailbox = xQueueCreate( 1, sizeof( uint8_t ) );
	/* Valor de selección inicial del encoder */
	uint8_t cValue = 0;
	xQueueOverwrite( xEncoderChoiceMailbox, &cValue );
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

	for ( ;; ) {
		/* Lectura de selección de motor en mailbox */
		xQueuePeek( xEncoderChoiceMailbox, &cValue, portMAX_DELAY );
		/* Inicio de mensaje */
		if ( cValue == 0 ) {
			strcpy( pcMsgToSend, ":S0D" );
			gpioWrite( LED2, ON );
			gpioWrite( LED3, OFF );
		} else if ( cValue == 1 ) {
			strcpy( pcMsgToSend, ":S1D" );
			gpioWrite( LED3, ON );
			gpioWrite( LED2, OFF );
		}

		/* Lectura de handle del set */
		xReceivedSemaphore = ( SemaphoreHandle_t ) xQueueSelectFromSet( xEncoderSemaphoreSet, portMAX_DELAY );
		/* Obtener que semáforo tiene información y realizar take */
		if ( xReceivedSemaphore == xEncoderPositivePulseSemaphore ) {
			xSemaphoreTake( xEncoderPositivePulseSemaphore, portMAX_DELAY );
			strcat( pcMsgToSend, "0" );
		} else if ( xReceivedSemaphore == xEncoderNegativePulseSemaphore ) {
			xSemaphoreTake( xEncoderNegativePulseSemaphore, portMAX_DELAY );
			strcat( pcMsgToSend, "1" );
		}

		/* Ángulo a setear */
		strcat( pcMsgToSend, "A15" );

		/* Enviar mensaje a cola de consignas */
		xQueueSendToBack(
			/* Handle de la cola a escribir */
			xStepperSetPointQueue,
			/* Puntero al dato a escribir */
			&pcMsgToSend,
			/* Máximo tiempo a esperar una escritura */
			portMAX_DELAY
		);
	}
}

/*! \fn BaseType_t xEncoderInit( void )
    \brief Inicialización de encoder rotativo de la aplicación.
*/
BaseType_t xEncoderInit( void )
{
	/* Configuracion de pines de la EDU-CIAA-NXP como entrada con pull-up */
	gpioConfig( encoderPIN_SW, GPIO_INPUT_PULLUP );
	gpioConfig( encoderPIN_CLK, GPIO_INPUT_PULLUP );
	gpioConfig( encoderPIN_DT, GPIO_INPUT_PULLUP );

	/*
	* Select irq channel to handle a GPIO interrupt, using its port and pin to specify it
	* From EduCiaa pin out spec: GPIO1[9] -> port 1 and pin 9
	*/
	Chip_SCU_GPIOIntPinSel( PININT1_INDEX, GPIO1_GPIO_PORT, GPIO1_GPIO_PIN );
	/* Clear actual configured interrupt status */
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
	/* Set edge interrupt mode */
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
	/* Enable high edge gpio interrupt */
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
	/* Clear pending irq channel interrupts */
	NVIC_ClearPendingIRQ( PIN_INT0_IRQn + PININT1_INDEX );
	/* Enable irqChannel interrupt */
	NVIC_EnableIRQ( PIN_INT0_IRQn + PININT1_INDEX );
	/* Seteo del nivel de prioridad de la interrupción 0 */
	NVIC_SetPriority( PININT1_NVIC_NAME, 255 );

	/*
	* Select irq channel to handle a GPIO interrupt, using its port and pin to specify it
	* From EduCiaa pin out spec: GPIO1[9] -> port 1 and pin 9
	*/
	Chip_SCU_GPIOIntPinSel( PININT2_INDEX, GPIO2_GPIO_PORT, GPIO2_GPIO_PIN );
	/* Clear actual configured interrupt status */
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );
	/* Set edge interrupt mode */
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );
	/* Enable high edge gpio interrupt */
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );
	/* Clear pending irq channel interrupts */
	NVIC_ClearPendingIRQ( PIN_INT0_IRQn + PININT2_INDEX );
	/* Enable irqChannel interrupt */
	NVIC_EnableIRQ( PIN_INT0_IRQn + PININT2_INDEX );
	/* Seteo del nivel de prioridad de la interrupción 0 */
	NVIC_SetPriority( PININT2_NVIC_NAME, 255 );

	/* Creación de tarea de procesamiento de información de encoder */
	BaseType_t xStatus;
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
