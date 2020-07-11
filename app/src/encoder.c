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

/* FreeRTOS includes */
#include "FreeRTOSPriorities.h"

/* Aplicación includes */
#include "encoder.h"

///*! \fn void PININT0_IRQ_HANDLER( void )
//	\brief Handler interrupt from GPIO pin or GPIO pin mapped to PININT
//*/
//void PININT0_IRQ_HANDLER( void )
//{
//	// Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT0_INDEX ) );
//}
//
//void PININT1_IRQ_HANDLER( void )
//{
//	// Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
//}

void PININT2_IRQ_HANDLER( void )
{
	BaseType_t xHigherPriorityTaskWoken;

	/* Clear del estado de interrupción */
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );

	gpioToggle( LEDB );
	printf("SWITCH PRESSED\r\n");

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

/*! \fn BaseType_t xEncoderInit( void )
    \brief Inicialización de encoder rotativo de la aplicación.
*/
BaseType_t xEncoderInit( void )
{
	/* Configuracion de entradas con pull-up conectadas a encoder */
	gpioConfig( encoderPIN_SW, GPIO_INPUT_PULLUP );
	gpioConfig( encoderPIN_CLK, GPIO_INPUT_PULLUP );
	gpioConfig( encoderPIN_DT, GPIO_INPUT_PULLUP );

	/* Funciones LPCOpen para configurar la interrupcion */
	/* Configuración canal de interrupción para el pin GPIO en bloque SysCon */
	Chip_SCU_GPIOIntPinSel( PININT0_INDEX, GPIO0_GPIO_PORT, GPIO0_GPIO_PIN );
	Chip_SCU_GPIOIntPinSel( PININT1_INDEX, GPIO1_GPIO_PORT, GPIO1_GPIO_PIN );
	Chip_SCU_GPIOIntPinSel( PININT2_INDEX, GPIO2_GPIO_PORT, GPIO2_GPIO_PIN );

	/* Configuración canal de interrupción como flanco sensible e interrupción
	por flanco de bajada */
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT0_INDEX) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT0_INDEX) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT0_INDEX) );

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT1_INDEX) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT1_INDEX) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT1_INDEX) );

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );

	/* Seteo del nivel de prioridad de la interrupción 0 */
	NVIC_SetPriority( PININT0_NVIC_NAME, 255 );
	/* Habilitar interrupción en NVIC 0 */
	NVIC_ClearPendingIRQ( PININT0_NVIC_NAME );
	NVIC_EnableIRQ( PININT0_NVIC_NAME );

	/* Seteo del nivel de prioridad de la interrupción 1 */
	NVIC_SetPriority( PININT1_NVIC_NAME, 255 );
	/* Habilitar interrupción en NVIC 1 */
	NVIC_ClearPendingIRQ( PININT1_NVIC_NAME );
	NVIC_EnableIRQ( PININT1_NVIC_NAME );

	/* Seteo del nivel de prioridad de la interrupción 2 */
	NVIC_SetPriority( PININT2_NVIC_NAME, 255 );
	/* Habilitar interrupción en NVIC 2 */
	NVIC_ClearPendingIRQ( PININT2_NVIC_NAME );
	NVIC_EnableIRQ( PININT2_NVIC_NAME );

	return pdPASS;
}
