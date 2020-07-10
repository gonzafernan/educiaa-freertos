/*! \file FreeRTOSPriorities.h
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

#ifndef FREERTOSPRIORITIES_H_
#define FREERTOSPRIORITIES_H_

#define priorityAppSyncTask			( configMAX_PRIORITIES - 5 )

#define priorityStepperControlTask	( configMAX_PRIORITIES - 4 )

#define priorityUartRxTask			( configMAX_PRIORITIES - 2 )
#define priorityUartTxTask			( configMAX_PRIORITIES - 3 )

#endif /* FREERTOSPRIORITIES_H_ */
