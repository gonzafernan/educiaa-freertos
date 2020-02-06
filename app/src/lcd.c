/*
 * lcd.c
 *
 *  Created on: Jan 30, 2020
 *      Author: Gonzalo G. Fernández
 */

#include "lcd.h"

/*-----------------------------------------------------------*/

void vTaskLCD( void *pvParameters )
{
	// Esperar que se estabilice la alimentación del LCD
	vTaskDelay( pdMS_TO_TICKS( LCD_STARTUP_WAIT_MS ) );

	// Inicializar LCD de 16x2 (caracteres x lineas) con cada caracter de 5x8 pixeles
	vLCDInit( 16, 2, 5, 8 );

	vLCDCursorSet( LCD_CURSOR_OFF );	// Apaga el cursor
	vLCDClear();						// Borrar la pantalla

	char msg[32];

	for( ;; )
	{
		vLCDGoToXY( 0, 0 ); // Poner cursor en 0, 0
		vLCDSendStringRaw( "Encoder:" );

		sprintf( msg, "%d", ENCODER_VALUE );

		vLCDGoToXY( 0, 1 ); // Poner cursor en 0, 1
		vLCDSendStringRaw( msg );

		vTaskDelay( pdMS_TO_TICKS( 20 ) );
		//vLCDClear();
	}
}

/*-----------------------------------------------------------*/

static void vLCDPinSet( uint8_t pin, bool_t status )
{
	gpioWrite( pin, status );
}

static void vLCDEnablePulse( void )
{
	// EN = 1 for H-to-L pulse
	vLCDPinSet( LCD_HD44780_EN, ON );

	// Wait to make EN wider
	vTaskDelay( pdMS_TO_TICKS( LCD_EN_PULSE_WAIT_US ) );

	// EN = 0 for H-to-L pulse
	vLCDPinSet( LCD_HD44780_EN, OFF );
}

void vLCDGoToXY( uint8_t x, uint8_t y )
{
	if( x >= lcd.lineWidth || y >= lcd.amountOfLines )
	{
		return;
	}

	uint8_t firstCharAdress[] = { 0x80, 0xC0, 0x94, 0xD4 };

	vLCDCommand( firstCharAdress[y] + x );
	vTaskDelay( pdMS_TO_TICKS( LCD_HIGH_WAIT_US ) );
	lcd.x = x;
	lcd.y = y;
}

void vLCDClearAndHome( void )
{
	vLCDClear();
	vLCDGoToXY( 0, 0 );
}

void vLCDClear( void )
{
	vLCDCommand( 0x01 );
	vTaskDelay( pdMS_TO_TICKS( LCD_CLR_DISP_WAIT_MS ) );
}

void vLCDCursorSet( lcdCursorModes_t mode )
{
	vLCDCommand( 0b00001100 | mode );
	vTaskDelay( pdMS_TO_TICKS( LCD_CLR_DISP_WAIT_MS ) );
}

void vLCDSendStringRaw( char *str )
{
	uint8_t i = 0;
	while( str[i] != 0 )
	{
		vLCDData( str[i] );
		i++;
	}
}

static void vLCDSendNibble( uint8_t nibble )
{
   vLCDPinSet( LCD_HD44780_D7, ( nibble & 0x80 ) );
   vLCDPinSet( LCD_HD44780_D6, ( nibble & 0x40 ) );
   vLCDPinSet( LCD_HD44780_D5, ( nibble & 0x20 ) );
   vLCDPinSet( LCD_HD44780_D4, ( nibble & 0x10 ) );
}

void vLCDCommand( uint8_t cmd )
{
	vLCDSendNibble( cmd & 0xF0 );		// Send high nibble to D7-D4

	vLCDPinSet( LCD_HD44780_RS, OFF );	// RS = 0 for command
	vLCDPinSet( LCD_HD44780_RW, OFF );	// RW = 0 for write

	vLCDEnablePulse();
	vTaskDelay( pdMS_TO_TICKS( LCD_LOW_WAIT_US ) );

	vLCDSendNibble( cmd << 4 );			// Send low nibble to D7-D4
	vLCDEnablePulse();
}

void vLCDData( uint8_t data )
{
	vLCDSendNibble( data & 0xF0 );	// Send high nibble to D7-D4

	vLCDPinSet( LCD_HD44780_RS, ON );	// RS = 1 for data
	vLCDPinSet( LCD_HD44780_RW, OFF );	// RW = 0 for write

	vLCDEnablePulse();

	vLCDSendNibble( data << 4 );	// Send low nibble to D7-D4
	vLCDEnablePulse();

}

void vLCDInit( uint16_t lineWidth, uint16_t amountOfLines,
		uint16_t charWidth, uint16_t charHeight )
{
	lcd.lineWidth = lineWidth;
	lcd.amountOfLines = amountOfLines;
	lcd.charWidth = charWidth;
	lcd.charHeight = charHeight;
	lcd.x = 0;
	lcd.y = 0;

	// Configure LCD Pins as Outputs
	lcdInitPinAsOutput( LCD_HD44780_RS );
	lcdInitPinAsOutput( LCD_HD44780_RW );
	lcdInitPinAsOutput( LCD_HD44780_EN );
	lcdInitPinAsOutput( LCD_HD44780_D4 );
	lcdInitPinAsOutput( LCD_HD44780_D5 );
	lcdInitPinAsOutput( LCD_HD44780_D6 );
	lcdInitPinAsOutput( LCD_HD44780_D7 );

	// Configure LCD for 4-bit mode
	vLCDPinSet( LCD_HD44780_RW, OFF );	// RW = 0
	vLCDPinSet( LCD_HD44780_RS, OFF );	// RS = 0
	vLCDPinSet( LCD_HD44780_EN, OFF );	// EN = 0

	vLCDCommand( 0x33 );	// Command 0x33 for 4-bit mode
	mLCDCommandDelay();

	vLCDCommand( 0x32 );	// Command 0x32 for 4-bit mode
	mLCDCommandDelay();

	vLCDCommand( 0x28 );	// Command 0x28 for 4-bit mode
	mLCDCommandDelay();

	// Initialize LCD
	vLCDCommand( 0x0E );	// Command 0x0E for display on, cursor on
	mLCDCommandDelay();

	vLCDClear();			// Command for clear LCD

	vLCDCommand( 0x06 );	// Command 0x06 for Shift cursor right
	mLCDCommandDelay();

	vTaskDelay( pdMS_TO_TICKS( 1 ) );
	vLCDCursorSet( LCD_CURSOR_OFF );

	vLCDClearAndHome();
}
