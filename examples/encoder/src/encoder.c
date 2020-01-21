
#include "sapi.h"
#include "chip.h"

// Para configurar cualquier pin hay que saber todos estos datos:
// Los que van en SCU: SCU_PORT, SCU_PIN, SCU_FUNC y los que van en GPIO:
// GPIO_PORT, GPIO_PIN

// SCU
#define GPIO0_SCU_PORT	6
#define GPIO0_SCU_PIN	1
#define GPIO0_SCU_FUNC	SCU_MODE_FUNC0

#define GPIO1_SCU_PORT	6
#define GPIO1_SCU_PIN	4
#define GPIO1_SCU_FUNC	SCU_MODE_FUNC0

#define GPIO2_SCU_PORT	6
#define GPIO2_SCU_PIN	5
#define GPIO2_SCU_FUNC	SCU_MODE_FUNC0

// GPIO
#define GPIO0_GPIO_PORT	3
#define GPIO0_GPIO_PIN	0

#define GPIO1_GPIO_PORT	3
#define GPIO1_GPIO_PIN	3

#define GPIO2_GPIO_PORT	3
#define GPIO2_GPIO_PIN	4

// Interrupt
#define PININT0_INDEX         0                  // PININT index used for GPIO mapping
#define PININT0_IRQ_HANDLER   GPIO0_IRQHandler   // GPIO interrupt IRQ function name
#define PININT0_NVIC_NAME     PIN_INT0_IRQn      // GPIO interrupt NVIC interrupt name

#define PININT1_INDEX         1                  // PININT index used for GPIO mapping
#define PININT1_IRQ_HANDLER   GPIO1_IRQHandler   // GPIO interrupt IRQ function name
#define PININT1_NVIC_NAME     PIN_INT1_IRQn      // GPIO interrupt NVIC interrupt name

#define PININT2_INDEX         2                  // PININT index used for GPIO mapping
#define PININT2_IRQ_HANDLER   GPIO2_IRQHandler   // GPIO interrupt IRQ function name
#define PININT2_NVIC_NAME     PIN_INT2_IRQn      // GPIO interrupt NVIC interrupt name


void PININT0_IRQ_HANDLER( void );
void PININT1_IRQ_HANDLER( void );
void PININT2_IRQ_HANDLER( void );


int VALUE = 0;


int main( void )
{

	/* ------------- INICIALIZACIONES ------------- */

	/* Inicializar la placa */
	boardConfig();

	/* Configuracion de GPIO0 de la EDU-CIAA-NXP como entrada con pull-up */
	gpioConfig( GPIO0, GPIO_INPUT_PULLUP );
	gpioConfig( GPIO1, GPIO_INPUT_PULLUP );
	gpioConfig( GPIO2, GPIO_INPUT_PULLUP );


	// ---> Comienzo de funciones LPCOpen para configurar la interrupcion

	// Configure interrupt channel for the GPIO pin in SysCon block
	Chip_SCU_GPIOIntPinSel( PININT0_INDEX, GPIO0_GPIO_PORT, GPIO0_GPIO_PIN );

	Chip_SCU_GPIOIntPinSel( PININT1_INDEX, GPIO1_GPIO_PORT, GPIO1_GPIO_PIN );

	Chip_SCU_GPIOIntPinSel( PININT2_INDEX, GPIO2_GPIO_PORT, GPIO2_GPIO_PIN );

	// Configure channel interrupt as edge sensitive and falling edge interrupt
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT0_INDEX) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT0_INDEX) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT0_INDEX) );

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT1_INDEX) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT1_INDEX) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT1_INDEX) );

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH(PININT2_INDEX) );

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( PININT0_NVIC_NAME );
	NVIC_EnableIRQ( PININT0_NVIC_NAME );

	NVIC_ClearPendingIRQ( PININT1_NVIC_NAME );
	NVIC_EnableIRQ( PININT1_NVIC_NAME );

	NVIC_ClearPendingIRQ( PININT2_NVIC_NAME );
	NVIC_EnableIRQ( PININT2_NVIC_NAME );


	for( ;; )
	{

	}

	return 0;
}

/*------------------------------------------------------------------------------*/

// Handler interrupt from GPIO pin or GPIO pin mapped to PININT
void PININT0_IRQ_HANDLER( void )
{


	// Se realiza alguna accion.
	gpioToggle(LEDB);

	printf("SWITCH PRESSED\r\n");

	// Se da aviso que se trato la interrupcion
	//delay(20);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT0_INDEX ) );
}


void PININT1_IRQ_HANDLER( void )
{
//	// Se da aviso que se trato la interrupcion
//	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT1_INDEX ) );
//
//	// Se realiza alguna accion.
//	printf("%d\r\n", &VALUE);
}

void PININT2_IRQ_HANDLER( void )
{

	// Se realiza alguna accion.

	if ( gpioRead( GPIO1 ) )
	{
		VALUE++;
	}
	else
	{
		VALUE--;
	}

	printf("%d\r\n", VALUE);

	volatile uint32_t ul; // Variable para loop for

	for ( ul = 0; ul < 0xffffff; ul++ )
	{
		/* Este bucle solo provoca un delay "forzado". */
	}
	// Se da aviso que se trato la interrupcion
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( PININT2_INDEX ) );

}

