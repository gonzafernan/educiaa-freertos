/*! \file display_lcd.h
    \brief Inicialización de display. Tarea para control de
    dicho módulo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Septiembre 2020
*/

#ifndef DISPLAY_LCD_H_
#define DISPLAY_LCD_H_

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
