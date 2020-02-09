/*
 * encoder.h
 *
 *  Created on: Feb 1, 2020
 *      Author: Gonzalo G. Fernández
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "sapi.h"
#include "chip.h"

#include "app.h"
#include "servo.h"

/*-----------------------------------------------------------*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "semphr.h"

#define ENCODER_PRIORITY	1

TaskHandle_t xEncoderTaskHandle;

/* Encoder task header */
void vTaskEncoder( void *pvParameters );

/*-----------------------------------------------------------*/

/* Definición de entradas del encoder */

#define ENC_SW	GPIO2
#define ENC_DT	GPIO3
#define ENC_CLK	GPIO4

/*-----------------------------------------------------------*/

/*
 * Para configurar cualquier pin hay que saber todos estos
 * datos: Los que van en SCU: SCU_PORT, SCU_PIN, SCU_FUNC y
 * los que van en GPIO: GPIO_PORT, GPIO_PIN
 *
 */

// SCU
#define GPIO2_SCU_PORT	6
#define GPIO2_SCU_PIN	5
#define GPIO2_SCU_FUNC	SCU_MODE_FUNC0

/*
#define GPIO3_SCU_PORT	6
#define GPIO3_SCU_PIN	7
#define GPIO3_SCU_FUNC	SCU_MODE_FUNC0
*/

#define GPIO4_SCU_PORT	6
#define GPIO4_SCU_PIN	8
#define GPIO4_SCU_FUNC	SCU_MODE_FUNC0

// GPIO
#define GPIO2_GPIO_PORT	3
#define GPIO2_GPIO_PIN	4

/*
#define GPIO3_GPIO_PORT	5
#define GPIO3_GPIO_PIN	15
*/

#define GPIO4_GPIO_PORT	5
#define GPIO4_GPIO_PIN	16

// Interrupt
#define PININT2_INDEX         2                  // PININT index used for GPIO mapping
#define PININT2_IRQ_HANDLER   GPIO2_IRQHandler   // GPIO interrupt IRQ function name
#define PININT2_NVIC_NAME     PIN_INT2_IRQn      // GPIO interrupt NVIC interrupt name

/*
#define PININT3_INDEX         3
#define PININT3_IRQ_HANDLER   GPIO3_IRQHandler
#define PININT3_NVIC_NAME     PIN_INT3_IRQn
*/

#define PININT4_INDEX         4                  // PININT index used for GPIO mapping
#define PININT4_IRQ_HANDLER   GPIO4_IRQHandler   // GPIO interrupt IRQ function name
#define PININT4_NVIC_NAME     PIN_INT4_IRQn      // GPIO interrupt NVIC interrupt name

void PININT2_IRQ_HANDLER( void );
// void PININT3_IRQ_HANDLER( void );
void PININT4_IRQ_HANDLER( void );

/*-----------------------------------------------------------*/

volatile int ENCODER_VALUE;

#endif /* ENCODER_H */
