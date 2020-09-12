/*! \file display_lcd.h
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Septiembre 2020

    Detalle.
*/

#ifndef DISPLAY_LCD_H_
#define DISPLAY_LCD_H_

/*! \var TaskHandle_t xDisplayTaskHandle
	\brief Handle de la tarea de control del Display LCD.
*/
extern TaskHandle_t xDisplayTaskHandle;

/*! \fn void vUpdateSelection( uint8_t cSelection )
	\brief Actualizar selección en el displat LCD.
	\param cSel Entero con el índice de la selección.
*/
void vUpdateSelection( uint8_t cSelection );

/*! \fn void vDisplayTask( void *pvParameters )
	\brief Tarea para control de display LCD.
*/
void vDisplayTask( void *pvParameters );

/*! \fn BaseType_t xDisplayInit( void )
	\brief Inicialización de módulo LCD
*/
BaseType_t xDisplayInit( void );

#endif /* DISPLAY_LCD_H_ */
