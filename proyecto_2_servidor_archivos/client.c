/** 
* @file
* @brief Cliente.
* @author Erwin Meza Vega <emezav@unicauca.edu.co>
* @copyright MIT License
*/
#include <arpa/inet.h>
#include <libgen.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "netutil.h"
#include "protocol.h"
#include "split.h"
#include "util.h"

/** @brief Carpeta donde el cliente guarda los archivos recibidos con get */
#define DOWNLOAD_DIR "client_files"

/** @brief Se activa al recibir SIGINT o SIGTERM */
static volatile sig_atomic_t finished = 0;

/**
 * @brief Manejador de las señales SIGINT (Ctrl+C) y SIGTERM
 * @param signal Señal recibida
 */
static void signal_handler(int signal) {
	if (signal == SIGINT || signal == SIGTERM) {
		//el manejador no hace nada mas que cambiar la bandera
		finished = 1;
	}
}

/**
 * @brief Envia una solicitud al servidor
 *
 * @param s Socket
 * @param operation Operacion (put, get, exit)
 * @param filename Nombre del archivo, o NULL si la operacion no lo requiere
 *
 * @return 0 en caso de exito, -1 si ocurre un error
 */
static int send_request(int s, const char * operation, const char * filename) {
	request req;
	memset(&req, 0, sizeof(request));
	strncpy(req.operation, operation, sizeof(req.operation) - 1);
	if (filename != NULL) {
		strncpy(req.filename, filename, sizeof(req.filename) - 1);
	}
	return write_all(s, &req, sizeof(request));
}

/**
 * @brief put: envia un archivo local al servidor
 *
 * @param s Socket
 * @param path Ruta del archivo local
 */
static void do_put(int s, char * path) {
	struct stat st;
	if (!file_exists(path) || stat(path, &st) != 0 || !S_ISREG(st.st_mode)) {
		fprintf(stderr, "'%s' no existe o no es un archivo regular\n", path);
		return;
	}

	// basename puede modificar su argumento, se usa una copia
	char copy[PATH_MAX];
	snprintf(copy, sizeof(copy), "%s", path);

	if (send_request(s, "put", basename(copy)) == -1) {
		perror("Error al enviar la solicitud");
		return;
	}

	response resp;
	if (read_all(s, &resp, sizeof(response)) == -1) {
		perror("Error al recibir la respuesta");
		return;
	}
	resp.message[sizeof(resp.message) - 1] = '\0';

	if (resp.code != TRANSFER_AUTHORIZED) {
		fprintf(stderr, "El servidor rechazo la transferencia: %s\n", resp.message);
		return;
	}

	if (send_file(s, path)) {
		printf("Archivo '%s' enviado\n", path);
	}
}

/**
 * @brief get: solicita un archivo al servidor
 *
 * @param s Socket
 * @param name Nombre del archivo en el servidor
 */
static void do_get(int s, char * name) {
	if (send_request(s, "get", name) == -1) {
		perror("Error al enviar la solicitud");
		return;
	}

	if (receive_file(s, DOWNLOAD_DIR)) {
		printf("Archivo '%s' recibido\n", name);
	}
}

int main(int argc, char * argv[]) {
	char * ip = (argc > 1) ? argv[1] : "127.0.0.1";
	unsigned short port = DEFAULT_PORT;
	if (argc > 2) {
		char * end;
		long p = strtol(argv[2], &end, 10);
		if (*end != '\0' || p < 1 || p > 65535) {
			fprintf(stderr, "Puerto invalido: %s\n", argv[2]);
			return EXIT_FAILURE;
		}
		port = (unsigned short)p;
	}

	// instalar el manejador para SIGINT y SIGTERM. Sin SA_RESTART, las llamadas
	// bloqueadas (fgets, recv, send) se interrumpen con EINTR y el cliente puede terminar
	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
		perror("Error al registrar sigaction");
		return EXIT_FAILURE;
	}

	// Directorio donde se almacenan los archivos recibidos: se crea si no existe
	// y se verifica que la ruta sea un directorio
	if (mkdir(DOWNLOAD_DIR, 0755) == -1) {
		struct stat st;
		if (errno != EEXIST || stat(DOWNLOAD_DIR, &st) != 0 || !S_ISDIR(st.st_mode)) {
			fprintf(stderr, "Error: '%s' no se pudo crear o no es un directorio\n", DOWNLOAD_DIR);
			return EXIT_FAILURE;
		}
	}

	// 1. Obtener un conector IPv4
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		perror("Error al crear el socket");
		return EXIT_FAILURE;
	}

	// 2. Conectar el socket a una direccion IPv4
	struct sockaddr_in * addr = address_by_ip(ip, port);
	if (addr == NULL) {
		fprintf(stderr, "Direccion IP invalida: %s\n", ip);
		close(s);
		return EXIT_FAILURE;
	}
	printf("Conectando al servidor...\n");
	int c = connect(s, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
	free(addr);
	if (c == -1) {
		perror("Error al conectar con el servidor");
		close(s);
		return EXIT_FAILURE;
	}
	printf("Conectado!...\n");

	// 3. Comunicacion: comandos put ARCHIVO, get ARCHIVO, exit
	char line[PATH_MAX + 16];
	int done = 0;
	while (!done && !finished) {
		printf("> ");
		fflush(stdout);
		if (fgets(line, sizeof(line), stdin) == NULL) {
			// fin de la entrada o señal de terminacion: se comporta como exit
			strcpy(line, "exit\n");
		}

		// split exige un delimitador al final, el '\n' de fgets lo cumple
		split_list * cmd = split(line, NULL);
		if (cmd->count == 0) {
			free_split_list(cmd);
			continue;
		}

		if (equals(cmd->parts[0], "exit") && cmd->count == 1) {
			send_request(s, "exit", NULL);
			done = 1;
		} else if (equals(cmd->parts[0], "put") && cmd->count == 2) {
			do_put(s, cmd->parts[1]);
		} else if (equals(cmd->parts[0], "get") && cmd->count == 2) {
			do_get(s, cmd->parts[1]);
		} else {
			fprintf(stderr, "Uso: put ARCHIVO | get ARCHIVO | exit\n");
		}
		free_split_list(cmd);
	}

	// 4. Cerrar la conexion con el servidor
	printf("Cerrando conexion...\n");
	close(s);

	exit(EXIT_SUCCESS);
}
