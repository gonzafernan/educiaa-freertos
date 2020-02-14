/*
 * @brief Funciones asociadas a la manipulación del servomotor
 * a través de la configuracion del SCT (State Configurable Timer)
 * para salida PWM.
 *
 * @note
 * No se utilizó las funciones específicas para motor servo
 * disponibles en la sAPI del firmware_v3 de la placa de desarrollo EDU_CIAA_NXP por
 * no ser compatibles con freeRTOS.
 * Las librerías dependencias son sAPI y LPCOpen
 *
 * servo.h
 *
 *  Created on: Feb 5, 2020
 *      Author: Gonzalo G. Fernández
 */

#ifndef SERVO_H
#define SERVO_H

/*-----------------------------------------------------------*/

#include "sapi.h"
#include "board.h"
#include "encoder.h"

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/*-----------------------------------------------------------*/

#define SERVO_ANGLE_MIN		-90		// Mínimo ángulo permitido
#define SERVO_ANGLE_MAX		90		// Máximo ángulo permitido

#define SERVO_PWM_PERIOD		20		// Periodo PWM en ms

#define SERVO_PULSE_MIN		1		// pulso en ms para menor posición (-90°)
#define SERVO_PULSE_MAX		2		// pulso en ms para mayor posición (+90°)

// duty-cycle para posición -90
#define SERVO_DUTYCYCLE_MIN	SERVO_PULSE_MIN*100/SERVO_PWM_PERIOD

// duty-cycle para posición +90
#define SERVO_DUTYCYCLE_MAX	SERVO_PULSE_MAX*100/SERVO_PWM_PERIOD

volatile uint16_t SERVO_VALUE;

void vServoInit( void );
void vServoStop( void );
int8_t isServoSetPosition( int32_t ilValue );

/*-----------------------------------------------------------*/

#define SCT_PWM				LPC_SCT
#define SCT_PWM_PIN_SERVO	1		// COUT1	Pin TFIL_1 o SERVO0
#define SCT_PWM_SERVO		2		// Index 2

// Frecuencia PWM 50Hz
#define SCT_PWM_RATE			50


#define SCU_PORT_SERVO		0x4		// Puerto 4 de SCU
#define SCU_PIN_SERVO		1		// Pin 1 de SCU

/*-----------------------------------------------------------*/

/* Prioridad de ka tarea RTOS asociada al control del servo motor */
#define SERVO_PRIORITY	2

TaskHandle_t xServoTaskHandle;

void vTaskServo( void *pvParameters );

/*-----------------------------------------------------------*/

#endif /* SERVO_H */
