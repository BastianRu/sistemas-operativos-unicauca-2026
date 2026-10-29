#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/limits.h>


/** @brief Puerto por defecto */
#define DEFAULT_PORT 1234 

/**
* @brief Solicitud del cliente
*/
typedef struct {
	char operation[16]; /**< Operacion a realizar (get, put, exit)*/
	char filename[NAME_MAX]; /**< Nombre del archivo a transferir */
}request;

/**
* @brief Codigos de respuesta
*/
typedef enum {
	TRANSFER_AUTHORIZED,
	TRANSFER_DENIED,
} response_code;

/**
* @brief Respuesta del servidor para verificar archivos repetidos
*/
typedef struct {
	response_code code; /**< Codigo de respuesta */
	char message[256]; /**< Mensaje (opcional) */
}response;
  
/**     
* @brief Informacion de un archivo a transferir
*/
typedef struct {
	off_t size; /**< Tamano del archivo a transferir, -1 si hay error ej. no existe*/
	char filename[NAME_MAX]; /**< Nombre del archivo */
	mode_t mode; /**< Modo */
}file_info;

//Nota: Se cambio de int a off_t para soporte de archivos grandes

/**
* @brief Envia exactamente len bytes por el socket
*
* @param s Socket
* @param buf Datos a enviar
* @param len Cantidad de bytes
*
* @return 0 en caso de exito, -1 si ocurre un error
*/
int write_all(int s, const void * buf, size_t len);

/**
* @brief Recibe exactamente len bytes del socket
*
* @param s Socket
* @param buf Buffer donde se almacenan los datos
* @param len Cantidad de bytes
*
* @return 0 en caso de exito, -1 si ocurre un error o se cierra la conexion
*/
int read_all(int s, void * buf, size_t len);

/**
* @brief Verifica que un nombre de archivo recibido sea seguro
*
* El nombre debe estar terminado en nulo (dentro de NAME_MAX), no ser vacio,
* ni "." o "..", ni contener '/'.
*
* @param name Nombre (arreglo de NAME_MAX bytes)
*
* @return 1 si es valido, 0 en caso contrario
*/
int valid_filename(const char * name);

/**
* @brief Recibe un archivo de un socket y lo almacena en la carpeta especificada
*
* @param s Socket
* @param destination_folder Carpeta en la cual se almacenara el archivo
*
* @return 1 en caso de exito, 0 si ocurre un error
*/
int receive_file(int s, char * destination_folder);
   
/**
* @brief Envia un archivo a un socket
*
* @param s Socket a enviar el archivo
* @param path Ruta al archivo a enviar
*
* @return 1 en caso de exito, 0 si ocurre un error
*/
int send_file(int s, char * path);

#endif
