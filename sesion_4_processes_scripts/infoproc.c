/**
 * @file infoproc.c
 * @brief Extracción de información de procesos en Linux leyendo /proc/[PID]/status mediante llamadas al sistema.
 * @details Este programa accede al pseudofilesystem procfs utilizando exclusivamente llamadas a bajo nivel
 *          (open, read, close) para extraer los campos Name, State, PPid y Threads de un proceso objetivo.
 * @author Tu Nombre
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/**
 * @brief Tamaño máximo del buffer de lectura para almacenar el contenido de /proc/[PID]/status.
 */
#define TAMANO_BUFFER 4096

/**
 * @brief Busca una etiqueta específica en el buffer y muestra su valor asociado por salida estándar.
 * 
 * @details Realiza una búsqueda de subcadena en memoria sobre el contenido del archivo de estado,
 *          omite tabuladores/espacios iniciales y extrae el valor hasta el siguiente salto de línea.
 * 
 * @param buffer Cadena de caracteres que contiene todo el texto leído de /proc/[PID]/status.
 * @param etiqueta Clave del campo a buscar (ejemplo: "Name:", "State:", "PPid:", "Threads:").
 * @param prefijo_salida Texto descriptivo impreso como prefijo del valor final extraído.
 */
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

/**
 * @brief Punto de entrada principal del programa infoproc.
 * 
 * @details Determina el PID objetivo (proporcionado por argv[1] o mediante getpid()),
 *          construye la ruta /proc/[PID]/status y realiza la lectura del archivo a bajo
 *          nivel empleando open, read y close con control de errores.
 * 
 * @param argc Cantidad de argumentos pasados desde la línea de comandos.
 * @param argv Arreglo de cadenas de texto con los argumentos. argv[1] representa opcionalmente el PID a consultar.
 * @return int Retorna 0 si la ejecución fue exitosa, 1 en errores de lectura/cierre o 2 si falla la apertura del archivo.
 */
int main(int argc, char *argv[]) {
    pid_t pid_objetivo;

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
