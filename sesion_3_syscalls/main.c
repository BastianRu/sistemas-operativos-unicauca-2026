/**
 * @file main.c
 * @brief Obtiene el PID y PPID del proceso y los muestra mediante write().
 *
 * Este programa obtiene el identificador del proceso actual mediante
 * getpid() y el identificador de su proceso padre mediante getppid().
 * Posteriormente, construye un mensaje y lo escribe en la salida estándar
 * utilizando la llamada al sistema write().
 *
 * El programa verifica el valor de retorno de write() y utiliza perror()
 * para informar posibles errores.
 *
 * @author Juan Sebastian Muñoz Ruiz
 * @author Juan Felipe Gallardo Orozco
 * @author Juan Pablo Medina Bolanios
 */

#include <stdio.h>
#include <unistd.h>

 /**
 * @brief Punto de entrada del programa.
 *
 * Obtiene el PID y PPID del proceso, construye un mensaje con estos valores
 * y lo escribe en la salida estándar mediante write().
 *
 * Si write() falla, se informa el error mediante perror() y el programa
 * termina con un código de error.
 *
 * @param argc Cantidad de argumentos recibidos desde la línea de comandos.
 * @param argv Arreglo que contiene los argumentos recibidos.
 *
 * @return 73 si el programa termina correctamente.
 * @return -1 si ocurre un error durante la escritura.
 */
int main(int argc, char * argv[]) {	

	int pid = getpid();

	if (pid == -1) {
		perror("Error al obtener el PID");
		return -1;
	}

	int ppid = getppid();
	if (ppid == -1) {
		perror("Error al obtener el PPID");
		return -1;
	}

	char buffer[100];
	int len = sprintf(buffer, "El pid es: %d y el ppid es: %d\n", pid, ppid);
	
	ssize_t wr = write(1, buffer, len);	
	
	if (wr == -1) {
		perror("Ha habido un error en la llamada al sistema");
		return -1;
	}

	return 73;
}   

