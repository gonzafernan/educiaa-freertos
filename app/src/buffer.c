/*! \file buffer.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Septiembre 2020

    Detalle.
*/

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

/* Aplicación includes */
#include "buffer.h"
#include "uart.h"

/*! \struct xBuffer_t
	\brief Definición de estructura del buffer circular,
	oculta al usuario.
*/
struct xBuffer_t {
	uint8_t** ppcBuffer;
	size_t xHead;
	size_t xTail;
	size_t xMax; //of the buffer
	/* Booleano para verificar si el buffer esta completo */
	BaseType_t xFull;
};

/*! \var uint8_t **ppcBufferCircular
	\brief Puntero a bloque de memoria destinado a buffer
	circular para mensajes de la aplicación.
*/
uint8_t **ppcBufferCircular;

/*! \var xBufferHandle_t xBufferMsgHandle
	\brief Handle del buffer circular para mensajes de la
	aplicación.
*/
xBufferHandle_t xBufferMsgHandle;

/*!
*/
SemaphoreHandle_t xBufferMutex;

/*! \var QueueHandle_t xMsgQueue
    \brief Cola de mensajes recibidos.
*/
QueueHandle_t xBufferMsgQueue;

/************************************************
 *  CREACIÓN DE BUFFER
 ***********************************************/

/*! \fn BaseType_t xBufferCreate( void )
	\brief Creación de buffer circular para mensajes de la
	aplicación.
	\return pdPASS si la creación se realizó con éxito.
*/
BaseType_t xBufferCreate( void )
{
	/* Asignación de memoria del buffer */
	//ppcBufferCircular = pvPortMalloc( bufferLENGTH * sizeof( uint8_t* ) );
	//for ( uint8_t i=0; i<bufferLENGTH; i++ ) {
	//	ppcBufferCircular[i] = pvPortMalloc( bufferMSG_LENGTH * sizeof( uint8_t ) );
	//}
	/* Inicialización de buffer circular */
	//xBufferMsgHandle = xBufferInit( ppcBufferCircular, bufferLENGTH );

	//xBufferMutex = xSemaphoreCreateMutex();

	/* Verificación de creación de buffer con éxito */
	//if ( xBufferMsgHandle && xBufferMutex ) {
	//	return pdPASS;
	//}

	//return pdFAIL;

	xBufferMsgQueue = xQueueCreate( bufferLENGTH, sizeof( char * ) );

	if ( xBufferMsgQueue ) {
		return pdPASS;
	}

	return pdFAIL;
}

/************************************************
 *  INICIALIZACIÓN Y RESET
 ***********************************************/

/*! \fn xBufferHandle_t xBufferInit(uint8_t* pcBuffer, size_t size)
	\brief Inicialización del buffer circular.
	\param ppcBuffer
	\param xSize
	\return Handle del buffer circular
*/
xBufferHandle_t xBufferInit( uint8_t** ppcBuffer, size_t xSize )
{
	/* Confirmación de que el buffer y tamaño son válidos */
	configASSERT( ppcBuffer && xSize );
	/* Alocación de memoria para el buffer circular */
	xBufferHandle_t xBufferHandle = pvPortMalloc( sizeof( xBuffer_t ) );
	/* Verificar que la alocación de memoria se realizó con éxito */
	configASSERT( xBufferHandle );

	/* Inicialización de parámetros de la estructura */
	xBufferHandle->ppcBuffer = ppcBuffer;
	xBufferHandle->xMax = xSize;

	/* Limpieza del buffer reseteándolo */
	vBufferReset( xBufferHandle );

	configASSERT( xBufferEmpty( xBufferHandle ) );

	return xBufferHandle;
}

/*! \fn void vBufferReset( xBufferHandle_t xBuffer )
	\brief Reseteo del buffer circular para vaciarlo.
	(head == tail).
	\param xBufferHandle Handle del buffer a resetear.
*/
void vBufferReset( xBufferHandle_t xBufferHandle )
{
	/* Verificación de que el buffer a resetear existe */
	configASSERT( xBufferHandle );

	/* Reinicialización del buffer circular */
	xBufferHandle->xHead = 0;
	xBufferHandle->xTail = 0;
	xBufferHandle->xFull = pdFALSE;
}

/*! \fn void vBufferFree( xBufferHandle_t xBuffer )
	\brief Eliminación del buffer circular.
	Liberación de memoria.
	\param xBufferHandle Handle del buffer a eliminar.
*/
void vBufferFree( xBufferHandle_t xBufferHandle )
{
	/* Verificación de que el buffer a eliminar existe */
	configASSERT( xBufferHandle );

	/* Liberación de memoria */
	vPortFree( xBufferHandle );
}

/************************************************
 *  CHEQUEO DE ESTADO
 ***********************************************/

/*! \fn BaseType_t xBufferFull( xBufferHandle_t xBuffer )
	\brief Verificación de buffer circular lleno.
	\param xBufferHandle Handle de buffer a verificar.
	\return Verdadero si el buffer esta lleno.
*/
BaseType_t xBufferFull( xBufferHandle_t xBufferHandle )
{
	/* Verificación de que el buffer a analizar existe */
	configASSERT( xBufferHandle );

	return xBufferHandle->xFull;
}

/*! \fn BaseType_t xBufferEmpty( xBufferHandle_t xBufferHandle )
	\brief Verificación de buffer circular vacío.
	\param xBufferHandle Handle de buffer a verificar.
	\return Verdadero si el buffer esta vacío.
*/
BaseType_t xBufferEmpty( xBufferHandle_t xBufferHandle )
{
	/* Verificación de que el buffer a analizar existe */
	configASSERT( xBufferHandle );

	return ( !xBufferHandle->xFull &&
			( xBufferHandle->xHead == xBufferHandle->xTail ) );
}

/*! \fn size_t xBufferCapacity( xBufferHandle_t xBufferHandle )
	\brief Obtener capacidad del buffer circular.
	\param xBufferHandle Handle de buffer a verificar.
	\return Capacidad del buffer (size_t)
*/
size_t xBufferCapacity( xBufferHandle_t xBufferHandle )
{
	/* Verificación de que el buffer a analizar existe */
	configASSERT( xBufferHandle );

	return xBufferHandle->xMax;
}

/*! \fn size_t xBufferSize( xBufferHandle_t xBufferHandle )
	\brief Obtener cantidad de elementos en el buffer.
	\param xBufferHandle Handle de buffer a verificar.
	\return Cantidad de elementos en buffer (size_t)
*/
size_t xBufferSize( xBufferHandle_t xBufferHandle )
{
	/* Verificación de que el buffer a analizar existe */
	configASSERT( xBufferHandle );

	size_t xSize = xBufferHandle->xMax;

	if ( !xBufferHandle->xFull ) {
		if ( xBufferHandle->xHead >= xBufferHandle->xTail ) {
			xSize = ( xBufferHandle->xHead - xBufferHandle->xTail );
		} else {
			xSize = ( xBufferHandle->xMax + xBufferHandle->xHead - xBufferHandle->xTail );
		}
	}

	return xSize;
}

/************************************************
 *  AÑADIR Y ELIMINAR INFORMACIÓN
 ***********************************************/

/*! \fn void vBufferAdvancePointer( xBufferHandle_t xBufferHandle )
	\brief Función interna para avanzar puntero.
	\param xBufferHandle Handle de buffer a modificar.
*/
static void prvBufferAdvancePointer( xBufferHandle_t xBufferHandle )
{
	/* Verificación de que el buffer a modificar existe */
	configASSERT( xBufferHandle );

	if ( xBufferHandle->xFull ) {
		xBufferHandle->xTail = ( xBufferHandle->xTail + 1 ) % xBufferHandle->xMax;
	}

	xBufferHandle->xHead = ( xBufferHandle->xHead + 1 ) % xBufferHandle->xMax;
	xBufferHandle->xFull = ( xBufferHandle->xHead == xBufferHandle->xTail );
}

/*! \fn void prvBufferRetreatPointer( xBufferHandle_t xBufferHandle )
	\brief Función interna para retroceder puntero.
	\param xBufferHandle Handle de buffer a modificar.
*/
static void prvBufferRetreatPointer( xBufferHandle_t xBufferHandle )
{
	/* Verificación de que el buffer a modificar existe */
	configASSERT( xBufferHandle );

	xBufferHandle->xFull = pdFALSE;
	xBufferHandle->xTail = ( xBufferHandle->xTail + 1 ) % xBufferHandle->xMax;
}

/*! \fn BaseType_t xBufferPut( xBufferHandle_t xBufferHandle, uint8_t *pcData )
	\brief Colocar elemento en el buffer circular.
	\param xBufferHandle Handle de buffer a modificar.
	\param pcData Elemento a escribir en el buffer.
	\return pdFAIL si el buffer se encuentra lleno (no se produce escritura).
*/
BaseType_t xBufferPut( xBufferHandle_t xBufferHandle, uint8_t *pcData )
{
	BaseType_t xStatus = pdFAIL;

	/* Verificación de que el buffer a escribir existe */
	configASSERT( xBufferHandle && xBufferHandle->ppcBuffer );

	if ( !xBufferFull( xBufferHandle ) ) {
		/* Escritua de elemento en el buffer */
		xBufferHandle->ppcBuffer[xBufferHandle->xHead] = pcData;
		/* Avanzar puntero del buffer */
		prvBufferAdvancePointer( xBufferHandle );

		xStatus = pdPASS;
	}

	return xStatus;
}

/*! \fn BaseType_t xBufferGet( xBufferHandle_t xBufferHandle, uint8_t *pcData )
	\brief Leer elemento del buffer circular.
	\param xBufferHandle Handle de buffer a modificar.
	\param pcData Elemento a leer del buffer.
	\return pdFAIL si el buffer se encuentra vacío (no se produce escritura).
*/
BaseType_t xBufferGet( xBufferHandle_t xBufferHandle, uint8_t* pcData )
{
	/* Verificación de que el buffer a leer existe */
	configASSERT( xBufferHandle && xBufferHandle->ppcBuffer );

	BaseType_t xStatus = pdFAIL;

	if ( !xBufferEmpty( xBufferHandle ) ) {
		/* Lectura de elemento en el buffer */
		pcData = xBufferHandle->ppcBuffer[xBufferHandle->xTail];
		/* Retroceder puntero del buffer */
		prvBufferRetreatPointer( xBufferHandle );

		xStatus = pdPASS;
	}

	return xStatus;
}

/************************************************
 *  ESCRITURA Y LECTURA DE MENSAJES
 ***********************************************/

void vBufferReadMsg( char* pcMsgToRead )
{
	//xSemaphoreTake( xBufferMutex, portMAX_DELAY );
	//xBufferGet( xBufferMsgHandle, pcMsgToRead );
	//xSemaphoreGive( xBufferMutex );
	xQueueReceive(
		/* Handle de la cola a leer */
		xBufferMsgQueue,
		/* Puntero a la memoria donde guardar lectura */
		&pcMsgToRead,
		/* Máximo tiempo que la tarea puede estar bloqueada
		esperando que haya información a leer */
		portMAX_DELAY
	);
}

void vBufferWriteMsg( char* pcMsgToWrite )
{
	//xSemaphoreTake( xBufferMutex, portMAX_DELAY );
	//xBufferPut( xBufferMsgHandle, pcMsgToWrite );
	//xSemaphoreGive( xBufferMutex );
	xQueueSendToBack(
		/* Handle de la cola a escribir */
		xBufferMsgQueue,
		/* Puntero al dato a escribir */
		&pcMsgToWrite,
		/* Máximo tiempo a esperar una escritura */
		portMAX_DELAY
	);
}
