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

/* Encoder source code */
#include "encoder.h"

/*Servo source code */
#include "servo.h"

/* Definición de LED vivo en GPIO0 */
#define LED_VIVO	GPIO0

/* Definición de buzzer en GPIO1 */
#define BUZZER		GPIO1
#define BUZZER_OFF	ON	// Salida en alto, buzzer apagado
#define BUZZER_ON	OFF	// Salida en bajo, buzzer encendido

void vTaskLED( void *pvParameters );
void vTaskBuzzer( void *pvParameters );

TaskHandle_t xLEDTaskHandle;
TaskHandle_t xBuzzerTaskHandle;

void vEncoderInit( void );

#endif /* APP_H */
