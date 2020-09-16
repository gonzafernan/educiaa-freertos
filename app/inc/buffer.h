/*! \file buffer.h
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Septiembre 2020

    Detalle.
*/

#ifndef BUFFER_H_
#define BUFFER_H_

/*! \def bufferLENGTH
	\brief Cantidad de mensajes que puede guardar el buffer circular.
*/
#define bufferLENGTH	10

/*! \def bufferMSG_LENGTH
	\brief Cantidad máxima de caracteres que puede tener un mensaje.
*/
#define bufferMSG_LENGTH	10

/*! \typedef xBuffer_t
	\brief Estructura del buffer circular.
*/
typedef struct xBuffer_t xBuffer_t;

/*! \typedef xBufferHandle_t
	\brief Handle para permitir la interacción del usuario
	con la API.
*/
typedef xBuffer_t* xBufferHandle_t;

/*! \fn BaseType_t xBufferCreate( void )
	\brief Creación de buffer circular para mensajes de la
	aplicación.
	\return pdPASS si la creación se realizó con éxito.
*/
BaseType_t xBufferCreate( void );

/*! \fn xBufferHandle_t xBufferInit(uint8_t* pcBuffer, size_t size)
	\brief Inicialización del buffer circular.
	\param ppcBuffer
	\param xSize
	\return Handle del buffer circular
*/
xBufferHandle_t xBufferInit( uint8_t** ppcBuffer, size_t xSize );

/*! \fn void vBufferFree( xBufferHandle_t xBuffer )
	\brief Eliminación del buffer circular.
	Liberación de memoria.
	\param xBufferHandle Handle del buffer a eliminar.
*/
void vBufferFree( xBufferHandle_t xBufferHandle );

/*! \fn void vBufferReset( xBufferHandle_t xBuffer )
	\brief Reseteo del buffer circular para vaciarlo.
	(head == tail).
	\param xBufferHandle Handle del buffer a resetear.
*/
void vBufferReset( xBufferHandle_t xBufferHandle );

/*! \fn BaseType_t xBufferPut( xBufferHandle_t xBufferHandle, uint8_t *pcData )
	\brief Colocar elemento en el buffer circular.
	\param xBufferHandle Handle de buffer a modificar.
	\param pcData Elemento a escribir en el buffer.
	\return pdFAIL si el buffer se encuentra lleno (no se produce escritura).
*/
BaseType_t xBufferPut( xBufferHandle_t xBufferHandle, uint8_t *pcData );

/*! \fn BaseType_t xBufferGet( xBufferHandle_t xBufferHandle, uint8_t *pcData )
	\brief Leer elemento del buffer circular.
	\param xBufferHandle Handle de buffer a modificar.
	\param pcData Elemento a leer del buffer.
	\return pdFAIL si el buffer se encuentra vacío (no se produce escritura).
*/
BaseType_t xBufferGet( xBufferHandle_t xBufferHandle, uint8_t* pcData );

/*! \fn BaseType_t xBufferEmpty( xBufferHandle_t xBufferHandle )
	\brief Verificación de buffer circular vacío.
	\param xBufferHandle Handle de buffer a verificar.
	\return Verdadero si el buffer esta vacío.
*/
BaseType_t xBufferEmpty( xBufferHandle_t xBufferHandle );

/*! \fn BaseType_t xBufferFull( xBufferHandle_t xBuffer )
	\brief Verificación de buffer circular lleno.
	\param xBufferHandle Handle de buffer a verificar.
	\return Verdadero si el buffer esta lleno.
*/
BaseType_t xBufferFull( xBufferHandle_t xBufferHandle );

/*! \fn size_t xBufferCapacity( xBufferHandle_t xBufferHandle )
	\brief Obtener capacidad del buffer circular.
	\param xBufferHandle Handle de buffer a verificar.
	\return Capacidad del buffer (size_t)
*/
size_t xBufferCapacity( xBufferHandle_t xBufferHandle );

/*! \fn size_t xBufferSize( xBufferHandle_t xBufferHandle )
	\brief Obtener cantidad de elementos en el buffer.
	\param xBufferHandle Handle de buffer a verificar.
	\return Cantidad de elementos en buffer (size_t)
*/
size_t xBufferSize( xBufferHandle_t xBufferHandle );

void vBufferReadMsg( char* pcMsgToRead );

void vBufferWriteMsg( char* pcMsgToWrite );

#endif /* APP_INC_BUFFER_H_ */
