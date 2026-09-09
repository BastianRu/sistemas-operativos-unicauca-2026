/**
 * @file procesos_hijos.c
 * @brief Programa en C que crea dos procesos hijos, ejecuta comandos distintos mediante execvp,
 *        y determina cual finaliza primero utilizando sincronizacion con wait().
 * @details Este programa demuestra el uso de bifurcacion de procesos con fork(),
 *          reemplazo de imagen de proceso con execvp(), y recoleccion de estados de salida 
 *          de los hijos para evitar procesos zombi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * @brief Funcion principal del programa.
 * @details Controla la creacion de dos procesos hijos mediante fork(), asigna tareas distintas 
 *          a cada uno usando execvp(), y sincroniza al padre para detectar la terminacion 
 *          del primer hijo y gestionar ambos estados de salida.
 * @return Retorna EXIT_SUCCESS en caso de ejecucion exitosa o EXIT_FAILURE si ocurre un error en fork/wait.
 */
int main(void) {
    /** Identificador de proceso para el primer hijo */
    pid_t pid1;

    /** Identificador de proceso para el segundo hijo */
    pid_t pid2;

    /* =========================================================================
     * 1. CREACIÓN Y EJECUCIÓN DEL PRIMER HIJO
     * ========================================================================= */
    pid1 = fork();

    if (pid1 < 0) {
        perror("Error al ejecutar fork en hijo 1");
        exit(EXIT_FAILURE);
    } 
    else if (pid1 == 0) {
        /* --- RAMA DEL HIJO 1 --- */
        printf("Hola, soy el hijo 1. Mi PID es %d, el PID de mi padre es %d.\n", 
               getpid(), getppid());

        printf("Voy a reemplazar mi imagen de proceso ejecutando 'factor 68719476736'\n");

        /** Argumentos para el comando factor */
        char *args[] = {"factor", "68719476736", NULL};
        execvp(args[0], args);

        /* Este codigo NUNCA se ejecutara si execvp tiene éxito */
        perror("Error al ejecutar execvp");
        printf("Este mensaje solo se imprime si execvp FALLA.\n");
        exit(EXIT_FAILURE);
    } 

    /* =========================================================================
     * 2. CREACIÓN Y EJECUCIÓN DEL SEGUNDO HIJO
     * ========================================================================= */
    pid2 = fork();

    if (pid2 < 0) {
        perror("Error al ejecutar fork en hijo 2");
        exit(EXIT_FAILURE);
    }
    else if (pid2 == 0) {
        /* --- RAMA DEL HIJO 2 --- */
        printf("Hola, soy el hijo 2. Mi PID es %d, el PID de mi padre es %d.\n", 
               getpid(), getppid());

        printf("Voy a reemplazar mi imagen de proceso ejecutando 'sleep 1'\n");

        /** Argumentos para el comando sleep */
        char *args[] = {"sleep", "1", NULL};
        execvp(args[0], args);

        /* Este codigo NUNCA se ejecutara si execvp tiene éxito */
        perror("Error al ejecutar execvp");
        printf("Este mensaje solo se imprime si execvp FALLA.\n");
        exit(EXIT_FAILURE);
    }

    /* =========================================================================
     * 3. RAMA DEL PADRE (SINCRONIZACIÓN Y EVALUACIÓN)
     * ========================================================================= */
    else {
        printf("Hola, soy el padre. Mi PID es %d, mi hijo 1 es %d y mi hijo 2 es %d.\n", 
               getpid(), pid1, pid2);

        /** Variable para almacenar la informacion de estado retornada por wait */
        int status;
        
        /** Almacena el PID del primer hijo que finaliza */
        pid_t primer_hijo = wait(&status);

        if (primer_hijo == -1) {
            perror("Error en wait");
            exit(EXIT_FAILURE);
        }

        /* Determinar cual de los dos hijos finalizo primero */
        if (primer_hijo == pid1) {
            printf("El **Hijo 1** (PID %d) termino PRIMERO.\n", primer_hijo);
        } else if (primer_hijo == pid2) {
            printf("El **Hijo 2** (PID %d) termino PRIMERO.\n", primer_hijo);
        }

        /* Analizar el estado de terminacion del primer hijo */
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("Termino normalmente con el codigo de salida: %d\n", exit_code);
        } else if (WIFSIGNALED(status)) {
            printf("Fue terminado por una senal: %d\n", WTERMSIG(status));
        }

        /* Esperar al hijo restante para evitar la creacion de procesos zombi */
        wait(NULL);
    }

    return EXIT_SUCCESS;
}
