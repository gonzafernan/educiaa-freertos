/*! \file driver_uln2003.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

/* Aplicación includes */
#include "../inc/driver_uln2003.h"

/*! \var DriverIn_t *pxDriverA
	\brief Arrays de entradas al driver del motor A
*/
DriverIn_t pxDriver[3][4] = {
		{ LCD3, LCD2, LCD1, GPIO3 },
		{ GPIO4, GPIO5, GPIO6, GPIO7 },
		{ T_COL0, T_FIL2, T_FIL3, T_FIL0 }
};

/*! \fn void vDriverUpdate( DriverIn_t *xDriverInput, uint8_t cState )
    \brief Escritura en driver de motor stepper dado un determinado estado.
    \param xDriverInput Arrays de entradas al driver a setear.
    \param cState Estado a escribir en el driver.
*/
void vDriverUpdate( DriverIn_t *xDriverInput, uint8_t cState )
{
    switch ( cState ) {
        case 0:
            gpioWrite(xDriverInput[0], ON);
            gpioWrite(xDriverInput[1], ON);
            gpioWrite(xDriverInput[2], OFF);
            gpioWrite(xDriverInput[3], OFF);
            break;

        case 1:
            gpioWrite(xDriverInput[0], OFF);
			gpioWrite(xDriverInput[1], ON);
			gpioWrite(xDriverInput[2], OFF);
			gpioWrite(xDriverInput[3], OFF);
            break;

        case 2:
            gpioWrite(xDriverInput[0], OFF);
			gpioWrite(xDriverInput[1], ON);
			gpioWrite(xDriverInput[2], ON);
			gpioWrite(xDriverInput[3], OFF);
            break;

        case 3:
            gpioWrite(xDriverInput[0], OFF);
			gpioWrite(xDriverInput[1], OFF);
			gpioWrite(xDriverInput[2], ON);
			gpioWrite(xDriverInput[3], OFF);
            break;

        case 4:
            gpioWrite(xDriverInput[0], OFF);
			gpioWrite(xDriverInput[1], OFF);
			gpioWrite(xDriverInput[2], ON);
			gpioWrite(xDriverInput[3], ON);
            break;

        case 5:
            gpioWrite(xDriverInput[0], OFF);
			gpioWrite(xDriverInput[1], OFF);
			gpioWrite(xDriverInput[2], OFF);
			gpioWrite(xDriverInput[3], ON);
            break;

        case 6:
            gpioWrite(xDriverInput[0], ON);
			gpioWrite(xDriverInput[1], OFF);
			gpioWrite(xDriverInput[2], OFF);
			gpioWrite(xDriverInput[3], ON);
            break;

        case 7:
            gpioWrite(xDriverInput[0], ON);
			gpioWrite(xDriverInput[1], OFF);
			gpioWrite(xDriverInput[2], OFF);
			gpioWrite(xDriverInput[3], OFF);
            break;
    }
}

/*! \fn void vDriverInit( DriverIn_t *xDriverInput, uint8_t cInputNum )
	\brief Inicialización de driver de motor stepper.
	\param vDriverInput Array con entradas al driver.
	\param cDriverNum Cantidad de entradas.
*/
void vDriverInit( DriverIn_t *xDriverInput, uint8_t cInputNum )
{
	/* Inicialización del pin en la placa asociado
	como salida */
	for ( uint8_t i=0; i<cInputNum; i++ ) {
		gpioInit( xDriverInput[i], GPIO_OUTPUT );
	}
}
