/**
 * @file informar.c
 * @brief Informa los metadatos de un archivo: tamano, tipo, permisos, enlaces duros y ultima modificacion.
 * @details Usa lstat(2), que no sigue enlaces simbolicos: sobre un enlace informa los metadatos
 *          del enlace mismo y no los del archivo al que apunta.
 *
 * Uso: `./informar <nombre_del_archivo>`
 */
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Imprime el tipo de archivo (regular, directorio, enlace simbolico, etc.).
 * @param modo Campo st_mode obtenido con lstat().
 */
void imprimir_tipo(mode_t modo);

/**
 * @brief Imprime los permisos en formato rwxrwxrwx (usuario, grupo, otros).
 * @param modo Campo st_mode obtenido con lstat().
 */
void imprimir_permisos(mode_t modo);

/**
 * @brief Punto de entrada: consulta el archivo con lstat() e imprime su informacion.
 * @param argc Cantidad de argumentos; debe ser 2.
 * @param argv argv[1] es la ruta del archivo a consultar.
 * @return EXIT_SUCCESS si lstat() tuvo exito, EXIT_FAILURE si faltan argumentos o lstat() falla.
 */
int main(int argc, char * argv[]) {
    // verificar argumentos
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_del_archivo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct stat info;

    //obtener informacion del archivo
    if (lstat(argv[1], &info) == -1) {
        perror("Error al obtener informacion del archivo");
        return EXIT_FAILURE;
    }

    //mostrar info
    printf("Archivo: %s\n", argv[1]);
    printf("Tamaño: %lld bytes\n", (long long)info.st_size);
    imprimir_tipo(info.st_mode);
    imprimir_permisos(info.st_mode);
    printf("Contador de enlaces duros: %lu\n", (unsigned long)info.st_nlink);

    char fecha[80];
    struct tm *tiempo_local = localtime(&info.st_mtime);
    strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tiempo_local);
    printf("ultima modificacion: %s\n", fecha);

    return EXIT_SUCCESS;
    
}

void imprimir_tipo(mode_t modo) {
    printf("Tipo de archivo: ");
    if (S_ISREG(modo))       printf("Archivo regular\n");
    else if (S_ISDIR(modo))  printf("Directorio\n");
    else if (S_ISLNK(modo))  printf("Enlace simbolico\n");
    else if (S_ISCHR(modo))  printf("Dispositivo de caracteres\n");
    else if (S_ISBLK(modo))  printf("Dispositivo de bloques\n");
    else if (S_ISFIFO(modo)) printf("Tuberia (FIFO/Pipe)\n");
    else if (S_ISSOCK(modo)) printf("Socket\n");
    else                     printf("Desconocido\n");
}

void imprimir_permisos(mode_t modo) {
    printf("Permisos: ");
    // Permisos de usuario 
    printf((modo & S_IRUSR) ? "r" : "-");
    printf((modo & S_IWUSR) ? "w" : "-");
    printf((modo & S_IXUSR) ? "x" : "-");
    // Permisos de grupo
    printf((modo & S_IRGRP) ? "r" : "-");
    printf((modo & S_IWGRP) ? "w" : "-");
    printf((modo & S_IXGRP) ? "x" : "-");
    // Permisos de otros
    printf((modo & S_IROTH) ? "r" : "-");
    printf((modo & S_IWOTH) ? "w" : "-");
    printf((modo & S_IXOTH) ? "x" : "-");
    printf("\n");
}


