/*
 * stepper.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Gonzalo G. Fernández
 */

#ifndef STEPPER_H
#define STEPPER_H

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "sapi.h"

typedef struct {
	uint8_t INT1;
	uint8_t INT2;
	uint8_t INT3;
	uint8_t INT4;
} stepper_t;

void vStepperInit( stepper_t *stepper, uint8_t pin[4] );

/* Tareas a crear */
void vTaskStepperControl( void *pvParameters );

#endif /* STEPPER_H */
