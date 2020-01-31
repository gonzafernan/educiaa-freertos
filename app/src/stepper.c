/*
 * stepper.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Gonzalo G. Fernández
 */

#include "stepper.h"

volatile uint8_t phaseControl = 0;
stepper_t *stp1;

void vStepperInit( stepper_t *stepper, uint8_t pin[4] )
{
	stepper->INT1 = pin[0];
	stepper->INT2 = pin[1];
	stepper->INT3 = pin[2];
	stepper->INT4 = pin[3];

	gpioInit(stepper->INT1, GPIO_OUTPUT);
	gpioInit(stepper->INT2, GPIO_OUTPUT);
	gpioInit(stepper->INT3, GPIO_OUTPUT);
	gpioInit(stepper->INT4, GPIO_OUTPUT);

}

void vTaskStepperControl( void *pvParameters )
{
	uint8_t stp1_pin[4] = {GPIO0, GPIO1, GPIO2, GPIO3};
	vStepperInit(stp1, stp1_pin);

	TickType_t xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
    	switch (phaseControl){
			case 0:
				gpioWrite(stp1->INT1, ON);
				gpioWrite(stp1->INT2, ON);
				gpioWrite(stp1->INT3, OFF);
				gpioWrite(stp1->INT4, OFF);
				break;

			case 1:
				gpioWrite(stp1->INT1, OFF);
				gpioWrite(stp1->INT2, ON);
				gpioWrite(stp1->INT3, OFF);
				gpioWrite(stp1->INT4, OFF);
				break;

			case 2:
				gpioWrite(stp1->INT1, OFF);
				gpioWrite(stp1->INT2, ON);
				gpioWrite(stp1->INT3, ON);
				gpioWrite(stp1->INT4, OFF);
				break;

			case 3:
				gpioWrite(stp1->INT1, OFF);
				gpioWrite(stp1->INT2, OFF);
				gpioWrite(stp1->INT3, ON);
				gpioWrite(stp1->INT4, OFF);
				break;

			case 4:
				gpioWrite(stp1->INT1, OFF);
				gpioWrite(stp1->INT2, OFF);
				gpioWrite(stp1->INT3, ON);
				gpioWrite(stp1->INT4, ON);
				break;

			case 5:
				gpioWrite(stp1->INT1, OFF);
				gpioWrite(stp1->INT2, OFF);
				gpioWrite(stp1->INT3, OFF);
				gpioWrite(stp1->INT4, ON);
				break;

			case 6:
				gpioWrite(stp1->INT1, ON);
				gpioWrite(stp1->INT2, OFF);
				gpioWrite(stp1->INT3, OFF);
				gpioWrite(stp1->INT4, ON);
				break;

			case 7:
				gpioWrite(stp1->INT1, ON);
				gpioWrite(stp1->INT2, OFF);
				gpioWrite(stp1->INT3, OFF);
				gpioWrite(stp1->INT4, OFF);
				break;
		}

    	if (phaseControl == 7){
			phaseControl = 0;
		} else {
			phaseControl++;
		}

    	vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 500 ) );
    }
}
