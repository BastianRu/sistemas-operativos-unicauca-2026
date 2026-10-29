# Planteamiento del problema

  Se deberá desarrollar dos programas en C que implementen la funcionalidad
  para enviar y recibir archivos a través de la red. Uno de los dos
  extremos actuará como cliente, y el otro como servidor. Quien recibe
  conexiones (el servidor) esperará por comandos del cliente, para transferir
  archivos desde o hacia el servidor.

  En el servidor, los archivos se almacenarán dentro de un directorio llamado
  "files", en la ubicación donde se encuentre el programa. Este directorio
  deberá ser creado si no existe.

  En el cliente, los archivos recibidos se almacenarán en el directorio
  "client_files, en la ubicación donde se encuentre el programa. Este directorio
  deberá ser creado si no existe.

  Se debe tener en cuenta que ambos extremos de comunicación, tanto el servidor,
  como el cliente, deben conocer la cantidad de bytes a transferir en cada
  operación *read-write*. Esto se debe a que ambas llamadas son bloqueantes, y
  esperan leer o escribir una cantidad determinada de bytes:

```c
#include <unistd.h>

// Read from a file descriptor
ssize_t read(int fd, void buf[.count], size_t count);

// Write to a file descriptor
ssize_t write(int fd, const void buf[.count], size_t count);
```

  Los comandos soportados por el cliente son:

* put ARCHIVO: Transfiere un archivo desde el cliente hacia el servidor. El
  cliente deberá enviar un mensaje al servidor con la solicitud (el nombre
  del archivo a transferir y su tamaño en bytes), seguido del contenido del
  archivo. El servidor recibirá el mensaje, y con la información
  suministrada, creará el archivo, leerá el contenido del socket y lo
  escribirá en el archivo (nuevo o existente) dentro del directorio ~files~.

* get ARCHIVO: Transfiere un archivo (que se ha almacenado anteriormente
  mediante PUT) desde el servidor hacia el cliente. El cliente deberá enviar
  un primer mensaje al servidor con la solicitud (el nombre del archivo a
  transferir), a lo cual el servidor responderá con un mensaje que contiene la
  información del archivo (por ejemplo, el tamaño en bytes), seguido del
  contenido del archivo. Si el archivo no existe, se deberá informar al
  cliente el error (por ejemplo, tamaño -1), y no se enviará nada a
  continuación de este mensaje.

* exit: El cliente deberá enviar un mensaje al servidor indicando que se va
  a cerrar la conexión. Tanto el cliente como el servidor terminarán la
  comunicación.

  Se debe permitir la comunicación entre un servidor y múltiples
  clientes, por lo cual en el servidor se deberá crear y administrar múltiples
  hilos, uno por cada conexión.

  También se debe considerar la situación en la cual varios clientes
  pueden tratar de enviar un archivo con el mismo nombre al servidor de forma
  simultánea. Si se presenta esta situación, se pueden tomar dos alternativas:
  rechazar el envío, o adicionar un sufijo al nombre del archivo guardado en el
  servidor (ej. *client_1.c*, *client_2.c*, etc.)

  Tanto el servidor como el cliente deberán terminar cuando se reciban las
  señales SIGTERM y SIGINT.

## Proceso servidor

  El servidor deberá permitir la conexión de múltiples clientes, por lo cual se
  deberán usar ~hilos~. Una posible estructura de datos que representa al
  cliente puede ser:

```c
/**
 * @brief Cliente
 */
typedef struct {
  int socket; /**< Descriptor del socket */
  int index; /**< Indice dentro del arreglo de clientes*/
  struct sockaddr_in address; /**< Direccion del cliente */
  char ip[INET_ADDRSTRLEN]; /*< Direccion IP en formato aaa.bbb.ccc.ddd **/
}client;
```

Cada vez que el servidor reciba una conexión mediante accept(2), deberá
crear un nuevo hilo y registrar el cliente en un arreglo. Este arreglo puede
ser definido con un tamaño estático, o crear a medida que se conectan más
clientes.

Cuando un hilo termina de antender a un cliente, se deberá liberar la
posición correspondiente en el arreglo.

En estas dos operaciones (registrar y quitar cliente), se deberá proteger el
acceso concurrente al arreglo de clientes, usando un ~mutex~.

## Condiciones de entrega

* Se deberá entregar un archivo comprimido en formato .zip o .tar.gz que
  contega los archivos de código fuente en C que implementan la
  solución del laboratorio. Se puede incluir archivos de cabecera y Makefile.
  También se debe incluir un archivo README.txt en el cual se incluyen los
  nombres, códigos y e-mail institucional de los autores.
* El código entregado debe compilar correctamente. En caso de no implementar
  la funcionalidad completa, se valorará proporcionalmente con el progreso
  alcanzado.
