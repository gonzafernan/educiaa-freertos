/*! \file driver_uln2003.h
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

#ifndef DRIVER_ULN2003_H_
#define DRIVER_ULN2003_H_

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

#define driverINPUT_NUM	4

/*! \var typedef gpioMap_t DriverIn_t
	\brief Tipo de dato entrada a driver.
*/
typedef gpioMap_t DriverIn_t;

/*! \var DriverIn_t *pxDriverA
	\brief Arrays de entradas al driver del motor A
*/
extern DriverIn_t pxDriver[2][4];

/*! \fn void vDriverUpdate( DriverIn_t *xDriverInput, uint8_t cState )
    \brief Escritura en driver de motor stepper dado un determinado estado.
    \param xDriverInput Arrays de entradas al driver a setear.
    \param cState Estado a escribir en el driver.
*/
void vDriverUpdate( DriverIn_t *xDriverInput, uint8_t cState );

/*! \fn void vDriverInit( void )
	\brief Inicialización de driver de motor stepper.
*/
void vDriverInit( DriverIn_t *xDriverInput, uint8_t cInputNum );

#endif /* DRIVER_ULN2003_H_ */
