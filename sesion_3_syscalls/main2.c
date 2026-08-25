/**
 * @file main2.c
 * @brief Imprime el mismo mensaje dos veces: primero con printf() y
 *        luego con write(), sin forzar el vaciado de buffers.
 *
 * Este programa ilustra la diferencia entre la salida con buffer de la
 * biblioteca estándar de C (printf) y la escritura directa mediante la
 * llamada al sistema write(). No se utiliza fflush() ni una terminación
 * que fuerce el vaciado de buffers, con el fin de observar el orden real
 * en que los datos llegan a la salida estándar.
 *
 * @author Juan Sebastian Muñoz Ruiz
 * @author Juan Felipe Gallardo Orozco
 * @author Juan Pablo Medina Bolanios
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

/**
 * @brief Punto de entrada del programa.
 *
 * Imprime el mismo mensaje primero con printf() y luego con write(),
 * en ese orden, sin forzar el vaciado de buffers.
 *
 * Si write() falla, se informa el error mediante perror() y el programa
 * termina con un código de error.
 *
 * @param argc Cantidad de argumentos recibidos desde la línea de comandos.
 * @param argv Arreglo que contiene los argumentos recibidos.
 *
 * @return 0 si el programa termina correctamente.
 * @return -1 si ocurre un error durante la escritura.
 */
int main(int argc, char *argv[]) {

    char *msg_printf = "Mensaje enviado con printf\n";
    char *msg_write  = "Mensaje enviado con write\n";

    printf("%s", msg_printf);

    ssize_t wr = write(1, msg_write, strlen(msg_write));
    if (wr == -1) {
        perror("Ha habido un error en la llamada al sistema");
        return -1;
    }

    return 0;
}