/*
 * app.h
 *
 *  Created on: Jan 25, 2020
 *      Author: ggf
 */

#ifndef APP_H
#define APP_H

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

/* sAPI library include. */
#include "sapi.h"

/* LCD source code */
#include "lcd.h"

/* Definición de LED vivo en GPIO0 */
#define LED_VIVO	GPIO0

void vTaskLED( void *pvParameters );


#endif /* APP_H */
