/*
 * lcd.h
 *
 *  Created on: Jan 30, 2020
 *      Author: ggf
 */

#ifndef LCD_H
#define LCD_H

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

/* sAPI library include. */
#include "sapi.h"

#define LCD_PRIORITY	2

void vTaskLCDInit( void *pvParameters );

void vTaskLCD( void *pvParameters );

/*-----------------------------------------------------------*/

#define mLCDCommandDelay()	vTaskDelay( pdMS_TO_TICKS( LCD_CMD_WAIT_US ) )

typedef struct {
   uint16_t lineWidth;
   uint16_t amountOfLines;
   uint16_t charWidth;
   uint16_t charHeight;
   uint8_t x;
   uint8_t y;
} lcd_t;

static lcd_t lcd;

static void vLCDPinSet( uint8_t pin, bool_t status );
static void vLCDEnablePulse( void );
void vLCDGoToXY( uint8_t x, uint8_t y );
void vLCDCommand( uint8_t cmd );
void vLCDClearAndHome( void );
void vLCDClear( void );
void vLCDCursorSet( lcdCursorModes_t mode );
void vLCDSendStringRaw( char *str );
static void vLCDSendNibble( uint8_t nibble );
void vLCDData( uint8_t data );
void vLCDInit( uint16_t lineWidth, uint16_t amountOfLines,
		uint16_t charWidth, uint16_t charHeight );

#endif /* LCD_H */
