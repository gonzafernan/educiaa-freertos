/*! \file encoder.h
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

#ifndef ENCODER_H_
#define ENCODER_H_

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "queue.h"

/*! \def encoderPIN_SW
	\brief Conexión pin switch de encoder rotativo.
*/
//#define encoderPIN_SW	GPIO2
#define encoderPIN_SW	GPIO0

// SCU
#define GPIO0_SCU_PORT	6
#define GPIO0_SCU_PIN	1
#define GPIO0_SCU_FUNC	SCU_MODE_FUNC0

// GPIO
#define GPIO0_GPIO_PORT	3
#define GPIO0_GPIO_PIN	0

// Interrupt
#define PININT_INDEX         0                  // PININT index used for GPIO mapping
#define PININT_IRQ_HANDLER   GPIO0_IRQHandler   // GPIO interrupt IRQ function name
#define PININT_NVIC_NAME     PIN_INT0_IRQn      // GPIO interrupt NVIC interrupt name


/*! \def encoderPIN_CLK
	\brief Conexión pin clock de encoder rotativo.
*/
#define encoderPIN_CLK	GPIO1

/*! \def encoderPIN_DT
	\brief Conexión pin dirección de encoder rotativo.
*/
#define encoderPIN_DT	GPIO0

/*! \def encoderMAX_CLK_PULSES
	\brief Cantidad máxima de pulsos admitidos por la aplicación.
*/
#define encoderMAX_CLK_PULSES	100

/*! \def encoderSTEP_TO_SEND
	\brief Cantidad de pasos a enviar con cada pulso de encoder.
*/
#define encoderANGLE_TO_SEND	15

#define GPIO1_SCU_PORT	6
#define GPIO1_SCU_PIN	4
#define GPIO1_SCU_FUNC	SCU_MODE_FUNC0

#define GPIO2_SCU_PORT	6
#define GPIO2_SCU_PIN	5
#define GPIO2_SCU_FUNC	SCU_MODE_FUNC0



#define GPIO1_GPIO_PORT	3
#define GPIO1_GPIO_PIN	3

#define GPIO2_GPIO_PORT	3
#define GPIO2_GPIO_PIN	4


#define PININT1_INDEX         1                  // PININT index used for GPIO mapping
#define vEncoderCLK_IRQ_HANDLER   GPIO1_IRQHandler   // GPIO interrupt IRQ function name
#define PININT1_NVIC_NAME     PIN_INT1_IRQn      // GPIO interrupt NVIC interrupt name

#define PININT2_INDEX         2                  // PININT index used for GPIO mapping
#define vEncoderSW_IRQ_HANDLER   GPIO2_IRQHandler   // GPIO interrupt IRQ function name
#define PININT2_NVIC_NAME     PIN_INT2_IRQn      // GPIO interrupt NVIC interrupt name

/*! \var QueueHandle_t xEncoderChoiceMailbox
	\brief Mailbox con la selección actual de motor mediante
	pulsador de encoder.
*/
extern QueueHandle_t xEncoderChoiceMailbox;

/*! \fn BaseType_t xEncoderInit( void )
    \brief Inicialización de encoder rotativo de la aplicación.
*/
BaseType_t xEncoderInit( void );

#endif /* ENCODER_H_ */
