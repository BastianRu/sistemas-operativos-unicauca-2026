#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("Error al ejecutar fork");
        exit(EXIT_FAILURE);
    } 

    else if (pid == 0) {
        printf("Hola, soy el hijo. Mi PID es %d, el PID de mi padre es %d.\n", 
               getpid(), getppid());

        printf("Voy a reemplazar mi imagen de proceso ejecutando 'factor '68719476736'\n");

        char *args[] = {"factor", "68719476736", NULL};
        execvp(args[0], args);

        // Este codigo NUNCA se ejecutara si execvp tiene exito
        perror("Error al ejecutar execvp");
        printf("Este mensaje solo se imprime si execvp FALLA.\n");
        exit(EXIT_FAILURE);
    } 

    else {
        printf("Hola, soy el padre. Mi PID es %d y mi hijo tiene el PID %d.\n", 
               getpid(), pid);

        int status;
        pid_t child_pid = waitpid(pid, &status, 0);

        if (child_pid == -1) {
            perror("Error en waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("Mi hijo (PID %d) termino normalmente con el codigo de salida: %d\n", 
                   child_pid, exit_code);
        } else if (WIFSIGNALED(status)) {
            printf("Mi hijo (PID %d) fue terminado por una senal: %d\n", 
                   child_pid, WTERMSIG(status));
        }
    }

    return EXIT_SUCCESS;
}
