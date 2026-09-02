#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define TAMANO_BUFFER 4096

void imprimir_valor_campo(char *buffer, char *etiqueta, char *prefijo_salida) {
    char *posicion = strstr(buffer, etiqueta);
    if (!posicion) {
        fprintf(stderr, "Error: No se encontró el campo %s\n", etiqueta);
        return;
    }

    posicion += strlen(etiqueta);

    while (*posicion == ' ' || *posicion == '\t') {
        posicion++;
    }

    char valor[256];
    size_t i = 0;
    while (*posicion != '\0' && *posicion != '\n' && i < sizeof(valor) - 1) {
        valor[i++] = *posicion++;
    }
    valor[i] = '\0';

    printf("%s %s\n", prefijo_salida, valor);
}

int main(int argc, char *argv[]) {
    pid_t pid_objetivo;

    printf("PID de este programa (getpid): %d\n", getpid());

    if (argc > 1) {
        pid_objetivo = atoi(argv[1]);
    } else {
        pid_objetivo = getpid();
    }

    char ruta[256];
    snprintf(ruta, sizeof(ruta), "/proc/%d/status", pid_objetivo);

    int fd = open(ruta, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo de status");
        exit(2);
    }

    char buffer[TAMANO_BUFFER];
    ssize_t bytes_totales = 0;
    ssize_t bytes_leidos = 0;

    while ((bytes_leidos = read(fd, buffer + bytes_totales, TAMANO_BUFFER - 1 - bytes_totales)) > 0) {
        bytes_totales += bytes_leidos;
        if (bytes_totales >= TAMANO_BUFFER - 1) {
            fprintf(stderr, "Error: El contenido del archivo supera el tamaño del buffer.\n");
            close(fd);
            exit(1);
        }
    }

    if (bytes_leidos == -1) {
        perror("Error al leer el archivo");
        close(fd);
        exit(1);
    }

    if (close(fd) == -1) {
        perror("Error al cerrar el archivo");
        exit(1);
    }

    buffer[bytes_totales] = '\0';

    imprimir_valor_campo(buffer, "Name:", "El nombre de este programa es");
    imprimir_valor_campo(buffer, "State:", "El estado de este programa es");
    imprimir_valor_campo(buffer, "PPid:", "El padre de este programa es");
    imprimir_valor_campo(buffer, "Threads:", "La cantidad de hilos de este programa es");

    return 0;
}
