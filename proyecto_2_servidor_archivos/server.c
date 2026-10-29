/** 
* @file
* @brief Servidor.
* @author Erwin Meza Vega <emezav@unicauca.edu.co>
* @copyright MIT License
*/
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "protocol.h"
#include "util.h"
#include "netutil.h"

/**
 * @brief Manejador de la señal SIGINT (Ctrl+C)
 * @param sig Señal recibida
 */
void sigint_handler(int signal);

/**
 * @brief Cliente
 */
typedef struct {
	int socket; /**< Descriptor del socket */
	int index; /**< Indice dentro del arreglo de clientes*/
	struct sockaddr_in address; /**< Direccion del cliente */
	char ip[INET_ADDRSTRLEN]; /*< Direccion IP en formato aaa.bbb.ccc.ddd **/
	int in_use; /**< 1 si la posicion esta ocupada por un cliente */
	int transferring; /**< 1 mientras el cliente atiende una solicitud */
}client;

#define MAXCLIENTS 256
/** @brief Arreglo de clientes (estatico), protegido por clients_mutex */
static client clients[MAXCLIENTS];

int nclients = 0; /**< Cantidad actual de clientes */

/** @brief Protege clients, nclients y el campo transferring de cada cliente */
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
/** @brief Se senala cuando nclients llega a 0 */
static pthread_cond_t clients_empty = PTHREAD_COND_INITIALIZER;

/**
 * @brief Hilo de cliente.
 *
 * @param arg Posicion en el arreglo de conectores de cliente
 *
 * @return NULL
 */
void * client_thread(void * arg);


/**
 * @brief Adiciona un socket de cliente al arreglo de clientes
 *
 * @param c Socket de conexion del cliente
 * @param addr Referencia a la direccion del cliente
 *
 * @return Apuntador a un nuevo cliente
 */
client * add_client(int c, struct sockaddr_in * addr);

/**
 * @brief Elimina un cliente
 *
 * @param c Apuntador al cliente
 */
void remove_client(client * c);



/**
 * @brief Marca el inicio de una solicitud de un cliente
 *
 * @param c Cliente
 *
 * @return 1 si puede atenderse, 0 si el servidor esta terminando
 */
static int begin_transfer(client * c);

/**
 * @brief Marca el fin de la solicitud de un cliente
 *
 * @param c Cliente
 */
static void end_transfer(client * c);

/**
 * @brief Atiende una solicitud (put, get, exit) de un cliente
 *
 * @param c Cliente
 * @param req Solicitud recibida
 *
 * @return 1 si la conexion puede continuar, 0 si debe cerrarse
 */
static int handle_request(client * c, request * req);

/** @brief Directorio donde el servidor guarda los archivos recibidos */
#define FILES_DIR "files"

volatile sig_atomic_t finished = 0;

int main(int argc, char * argv[]) {
    // el manejador 
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask); //esto de aqui es para bloquear otras señales mientras se ejecuta el manejador
    sa.sa_flags = 0;
    // instalar el manejador para SIGINT y SIGTERM
    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
            perror("Error al registrar sigaction");
            return EXIT_FAILURE;
    }

    // Directorio donde se almacenan los archivos recibidos: se crea si no existe
    // y se verifica que la ruta sea un directorio
    if (mkdir(FILES_DIR, 0755) == -1) {
        struct stat st;
        if (errno != EEXIST || stat(FILES_DIR, &st) != 0 || !S_ISDIR(st.st_mode)) {
            fprintf(stderr, "Error: '%s' no se pudo crear o no es un directorio\n", FILES_DIR);
            return EXIT_FAILURE;
        }
    }

	int s, c; // Conectores
	struct sockaddr_in addr; //Direccion IPv4 del socket del servidor
	struct sockaddr_in client_addr; //Direccion IPv4 del cliente
	socklen_t addr_len; // Tamaño de la direccion 

	// 1. Obtener un conector IPv4
	s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("Error al crear el socket");
        return EXIT_FAILURE;
    }
	// 2. Asociar el socket a una direccion IPv4
	// inicializar la direccion
	memset(&addr, 0, sizeof(struct sockaddr_in));
	//Asignar los atributos a la estructura
	addr.sin_family = AF_INET; //Corresponde al tipo de socket (paso 1)
	addr.sin_port = htons(1234); //TODO linea de comandos!
	if (inet_aton("0.0.0.0", &addr.sin_addr) == 0) { //escuchar en la IP que me asignen
        fprintf(stderr, "Error: direccion IP invalida\n");
        close(s);
        return EXIT_FAILURE;
    }

    int b;
    b = bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (b == -1) {
        perror("Error al asociar el socket a la direccion");
        close(s);
        return EXIT_FAILURE;
    } 

	// 3. Poner el socket disponible
    int l;
    l = listen(s, 10);
    if (l == -1) {
        perror("Error al poner el socket en modo escucha");
        close(s);
        return EXIT_FAILURE;
    }
	// Los hilos de clientes no se esperan con join: se liberan solos al terminar
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	// Inicio de estructura repetitiva
	// para permitir la conexion con multiples clientes
	while (!finished) {
		// 4. Esperar por un cliente
		printf("Esperando una conexion..\n");
		addr_len = sizeof(client_addr); // accept lo usa como entrada y salida
		c = accept(s, (struct sockaddr *)&client_addr, &addr_len);

        //de igual forma hay que comprobar si el accept fue interrumpido, y tambien
        // si se recibio una señal de terminacion, en cuyo caso se sale del bucle
        if (c == -1) {
            if (finished && errno == EINTR) {
                // si se recibio una señal de terminacion y efectivamente es ctrl+c, salir del bucle
                break;
            } else if (!finished && errno == EINTR) {
                continue; // volver a evaluar el ciclo
            } else {
                perror("Error al aceptar la conexion");
                finished = 1; // los hilos de clientes tambien deben terminar
                break;
            }
        }
		// 5. Registrar al cliente y crear un hilo para atenderlo
		client * cl = add_client(c, &client_addr);
		if (cl == NULL) {
			fprintf(stderr, "Maximo de clientes alcanzado, conexion rechazada\n");
			close(c);
			continue;
		}
		printf("Cliente conectado!!.. %s\n", cl->ip);

		// El hilo hereda la mascara: SIGINT y SIGTERM solo las recibe el hilo principal.
		// Se bloquean antes de crearlo para que no exista un instante en que las pueda capturar.
		sigset_t blocked, previous;
		sigemptyset(&blocked);
		sigaddset(&blocked, SIGINT);
		sigaddset(&blocked, SIGTERM);
		pthread_sigmask(SIG_BLOCK, &blocked, &previous);

		pthread_t tid;
		int e = pthread_create(&tid, &attr, client_thread, cl);

		pthread_sigmask(SIG_SETMASK, &previous, NULL);

		if (e != 0) {
			fprintf(stderr, "Error al crear el hilo: %s\n", strerror(e));
			remove_client(cl);
		}
	}

	// 6. Terminar: los clientes inactivos se desconectan, los que transfieren terminan
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAXCLIENTS; i++) {
		if (clients[i].in_use && !clients[i].transferring) {
			// desbloquea el read del hilo, que sale y se quita del arreglo
			shutdown(clients[i].socket, SHUT_RDWR);
		}
	}
	while (nclients > 0) {
		pthread_cond_wait(&clients_empty, &clients_mutex);
	}
	pthread_mutex_unlock(&clients_mutex);
	pthread_attr_destroy(&attr);

	// 7. Cerrar el socket obtenido en el paso 1.

	printf("Cerrando socket...\n");
	close(s);

	exit(EXIT_SUCCESS);
}

client * add_client(int c, struct sockaddr_in * addr) {
	client * ret = NULL;

	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAXCLIENTS; i++) {
		if (!clients[i].in_use) {
			ret = &clients[i];
			ret->socket = c;
			ret->index = i;
			ret->address = *addr;
			inet_ntop(AF_INET, &addr->sin_addr, ret->ip, sizeof(ret->ip));
			ret->in_use = 1;
			ret->transferring = 0;
			nclients++;
			break;
		}
	}
	pthread_mutex_unlock(&clients_mutex);

	return ret;
}

void remove_client(client * c) {
	pthread_mutex_lock(&clients_mutex);
	// el close va bajo el mutex: el hilo principal solo hace shutdown de posiciones ocupadas
	close(c->socket);
	c->in_use = 0;
	nclients--;
	if (nclients == 0) {
		pthread_cond_broadcast(&clients_empty);
	}
	pthread_mutex_unlock(&clients_mutex);
}

static int begin_transfer(client * c) {
	int ok;

	pthread_mutex_lock(&clients_mutex);
	ok = !finished;
	if (ok) {
		c->transferring = 1;
	}
	pthread_mutex_unlock(&clients_mutex);

	return ok;
}

static void end_transfer(client * c) {
	pthread_mutex_lock(&clients_mutex);
	c->transferring = 0;
	pthread_mutex_unlock(&clients_mutex);
}

void * client_thread(void * arg) {
	client * c = (client *)arg;
	request req;

	while (!finished) {
		// si el cliente cierra la conexion, o el servidor termina, read_all falla
		if (read_all(c->socket, &req, sizeof(request)) == -1) {
			break;
		}
		if (!begin_transfer(c)) {
			break;
		}
		int keep = handle_request(c, &req);
		end_transfer(c);
		if (!keep) {
			break;
		}
	}

	printf("Cerrando conexion con %s...\n", c->ip);
	remove_client(c);

	return NULL;
}

static int handle_request(client * c, request * req) {
	req->operation[sizeof(req->operation) - 1] = '\0';

	if (equals(req->operation, "put")) {
		response resp;
		memset(&resp, 0, sizeof(response));

		// el nombre se valida antes de autorizar, para que el cliente no envie el contenido
		if (!valid_filename(req->filename)) {
			resp.code = TRANSFER_DENIED;
			snprintf(resp.message, sizeof(resp.message), "Nombre de archivo invalido");
			return write_all(c->socket, &resp, sizeof(response)) == 0;
		}

		resp.code = TRANSFER_AUTHORIZED;
		snprintf(resp.message, sizeof(resp.message), "OK");
		if (write_all(c->socket, &resp, sizeof(response)) == -1) {
			return 0;
		}

		// si falla, el flujo queda a medias y la conexion no se puede reutilizar
		return receive_file(c->socket, FILES_DIR);

	} else if (equals(req->operation, "get")) {
		char path[PATH_MAX];
		struct stat st;

		if (!valid_filename(req->filename) ||
				snprintf(path, sizeof(path), "%s/%s", FILES_DIR, req->filename) >= (int)sizeof(path) ||
				stat(path, &st) != 0 || !S_ISREG(st.st_mode) ||
				access(path, R_OK) != 0) { // sin permiso de lectura: aun se esta recibiendo
			// size = -1 indica que no existe; no se envia nada mas y la conexion continua
			file_info fi;
			memset(&fi, 0, sizeof(file_info));
			fi.size = -1;
			return write_all(c->socket, &fi, sizeof(file_info)) == 0;
		}

		return send_file(c->socket, path);

	} else if (equals(req->operation, "exit")) {
		return 0;
	}

	fprintf(stderr, "Solicitud desconocida de %s\n", c->ip);
	return 0;
}

void sigint_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        //el manejador no hace nada mas que cambiar la bandera
        finished = 1;
    }
}
