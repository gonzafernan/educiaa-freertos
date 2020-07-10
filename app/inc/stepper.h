/*! \file stepper.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

#ifndef STEPPER_H
#define STEPPER_H

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "FreeRTOSPriorities.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

/* EDU-CIAA firmware_v3 includes */
#include "sapi.h"

/*! \def stepperAPP_NUM
    \brief Cantidad de motores en la aplicación
*/
#define stepperAPP_NUM  1

/*! \def stepperTIMER_PERIOD
    \brief Periodo de pasos de los motores (velocidad del motor)
*/
#define stepperTIMER_PERIOD 2

/*! \def stepperDIR_NEGATIVE
    \brief Dirección negativa del motor (depende de conexión del driver)
*/
#define stepperDIR_NEGATIVE 0

/*! \def stepperDIR_POSITIVE
    \brief Dirección negativa del motor (depende de conexión del driver)
*/
#define stepperDIR_POSITIVE 1

/*! \def stepperMAX_SETPOINT_QUEUE_LENGTH
    \brief Máxima cantidad de consignas que se puede almacenar en buffer.
*/
#define stepperMAX_SETPOINT_QUEUE_LENGTH    10

/*! \var typedef char StepperDir_t
    \brief Definición de tipo para dirección de motores stepper.
*/
typedef char StepperDir_t;

/*! \var QueueHandle_t xStepperSetPointQueue
    \brief Cola de consignas recibidas a ejecutar.
*/
extern QueueHandle_t xStepperSetPointQueue;

/*! \fn void vStepperRelativeSetPoint( TimerHandle_t xStepperTimer, int32_t lRelativeSetPoint )
    \brief Setear una nueva consigna en motor stepper relativa a la posición actual.
    \param xStepperTimer Handle del timer asociado al motor que se desea asignar la nueva consigna.
    \param ulRelativeSetPoint Cantidad de pasos a realizar.
    \param xStepperDir Dirección de los pasos a realizar.
*/
BaseType_t xStepperRelativeSetPoint( TimerHandle_t xStepperTimer, uint32_t ulRelativeSetPoint, StepperDir_t xStepperDir );

/*! \fn BaseType_t xStepperInit( void )
    \brief Inicialización de motores de la aplicación.
*/
BaseType_t xStepperInit( void );

#endif /* STEPPER_H_ */
