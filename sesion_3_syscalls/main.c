#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char * argv[]) {	

	int pid = getpid();
	int ppid = getppid();

	char buffer[100];
	int len = sprintf(buffer, "El pid es: %d y el ppid es: %d\n", pid, ppid);
	
	ssize_t wr = write(1, buffer, len);	
	
	if (wr == -1) {
		perror("Ha habido un error en la llamada al sistema");
		return -1;
	}

	return 73;
}   

