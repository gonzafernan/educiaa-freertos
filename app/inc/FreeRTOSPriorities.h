/*! \file FreeRTOSPriorities.h
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

#ifndef FREERTOSPRIORITIES_H_
#define FREERTOSPRIORITIES_H_

#define priorityLedBlinkTask			( configMAX_PRIORITIES - 5 )

#define priorityAppSyncTask			( configMAX_PRIORITIES - 5 )

#define priorityStepperControlTask	( configMAX_PRIORITIES - 4 )
#define priorityServoControlTask		( configMAX_PRIORITIES - 4 )

#define priorityUartRxTask			( configMAX_PRIORITIES - 2 )
#define priorityUartTxTask			( configMAX_PRIORITIES - 3 )

#define priorityEncoderTask			( configMAX_PRIORITIES - 5 )

#define priorityDisplayTask			( configMAX_PRIORITIES - 6 )

#endif /* FREERTOSPRIORITIES_H_ */
