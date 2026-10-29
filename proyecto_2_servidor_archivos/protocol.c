#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"

/** @brief Tamano de los bloques de lectura al enviar un archivo */
#define BLOCK_SIZE 4096

//se declara como void para aceptar cualquier tipo de buffer
int write_all(int s, const void * buf, size_t len) {
    size_t bytes_sent = 0;
    ssize_t bytes_sent_i = 0;

    while (bytes_sent < len) {
        bytes_sent_i = send(s, (uint8_t *)buf + bytes_sent, len - bytes_sent, MSG_NOSIGNAL);
        if (bytes_sent_i == -1) {
            return -1; // error
        }
        bytes_sent += bytes_sent_i;
    }

    return 0; 
}

int read_all(int s, void * buf, size_t len) {
    size_t bytes_received = 0;
    ssize_t bytes_received_i = 0;

    while (bytes_received < len) {
        bytes_received_i = recv(s, (uint8_t *)buf + bytes_received, len - bytes_received, 0);
        if (bytes_received_i == -1) {
            return -1; // error
        } else if (bytes_received_i == 0) {
            // connection closed
            return -1;
        }
        bytes_received += bytes_received_i;
    }

    return 0; 
}

int valid_filename(const char * name) {
    // debe estar terminado en '\0', no ser vacio, ni "." / "..",
    // y no contener '/' para que no escape del directorio de destino
    return memchr(name, '\0', NAME_MAX) != NULL && name[0] != '\0' &&
        strcmp(name, ".") != 0 && strcmp(name, "..") != 0 &&
        strchr(name, '/') == NULL;
}

int receive_file(int s, char * destination_folder) {
    // destination_folder ya fue validado por el servidor al arrancar (existe y es directorio)
    file_info fi;
    int r = read_all(s, &fi, sizeof(file_info));
    if (r == -1) {
        perror("Error al recibir file_info");
        return 0;
    }

    if (!valid_filename(fi.filename)) {
        fprintf(stderr, "Nombre de archivo invalido\n");
        return 0;
    }

    // validar el tamano (-1 indica error en el otro extremo)
    if (fi.size < 0) {
        fprintf(stderr, "Tamano de archivo invalido: %lld\n", (long long)fi.size);
        return 0;
    }

    // open con O_CREAT | O_EXCL falla con EEXIST si el archivo ya existe;
    // en ese caso se reintenta con el sufijo _1, _2, ... antes de la extension (foto_1.png).
    // Un punto inicial (".bashrc") no cuenta como extension
    const char * dot = strrchr(fi.filename, '.');
    int stem_len = (dot == NULL || dot == fi.filename) ? (int)strlen(fi.filename) : (int)(dot - fi.filename);
    const char * ext = fi.filename + stem_len;
    char path[PATH_MAX];
    char name[NAME_MAX];
    int fd = -1;
    for (unsigned int n = 0; fd == -1; n++) {
        int len;
        if (n == 0) {
            len = snprintf(name, sizeof(name), "%s", fi.filename);
        } else {
            len = snprintf(name, sizeof(name), "%.*s_%u%s", stem_len, fi.filename, n, ext);
        }
        if (len < 0 || (size_t)len >= sizeof(name)) {
            fprintf(stderr, "El nombre del archivo es demasiado largo\n");
            return 0;
        }

        len = snprintf(path, sizeof(path), "%s/%s", destination_folder, name);
        if (len < 0 || (size_t)len >= sizeof(path)) {
            fprintf(stderr, "La ruta del archivo es demasiado larga\n");
            return 0;
        }

        // se crea solo con permiso de escritura: mientras se recibe no puede leerse (get) incompleto
        fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0200);
        if (fd == -1 && errno != EEXIST) {
            perror("Error al crear el archivo");
            return 0;
        }
    }

    // leer exactamente fi.size bytes del socket y escribirlos en el archivo, en bloques
    char block[BLOCK_SIZE];
    off_t remaining = fi.size;
    while (remaining > 0) {
        size_t to_read = remaining < (off_t)sizeof(block) ? (size_t)remaining : sizeof(block);
        if (read_all(s, block, to_read) == -1) {
            perror("Error al recibir el contenido");
            goto failure;
        }

        size_t written = 0;
        while (written < to_read) {
            ssize_t n = write(fd, block + written, to_read - written);
            if (n == -1) {
                if (errno == EINTR) {
                    continue;
                }
                perror("Error al escribir el archivo");
                goto failure;
            }
            written += n;
        }
        remaining -= to_read;
    }

    // archivo completo: se le asignan los permisos originales
    if (fchmod(fd, fi.mode & 0777) == -1) {
        perror("Error al asignar los permisos");
        goto failure;
    }

    close(fd);
    return 1;

failure:
    // no dejar archivos incompletos en el directorio
    close(fd);
    unlink(path);
    return 0;
}

int send_file(int s, char * path) {
    struct stat buffer;

    if (stat(path, &buffer) == 0) {
        //si existe la ruta
        if (S_ISREG(buffer.st_mode)) {
            //si es archivo regular entonces se envia un file_info con la informacion del archivo
            file_info fi;
            fi.size = buffer.st_size;
            memset(fi.filename, 0, NAME_MAX);
            strncpy(fi.filename, basename(path), NAME_MAX - 1); // copiar el nombre al struct file_info
            fi.mode = buffer.st_mode;

            //abrir el archivo antes de enviar la cabecera, para poder avisar si no se puede leer
            int fd = open(path, O_RDONLY);
            if (fd == -1) {
                perror("Error al abrir el archivo");
                fi.size = -1; // el otro extremo no debe esperar contenido
                write_all(s, &fi, sizeof(file_info));
                return 0;
            }

            //enviar file_info al socket, esto es la cabecera, luego va el contenido
            int w = write_all(s, &fi, sizeof(file_info));
            if (w == -1) {
                perror("Error al enviar file_info");
                close(fd);
                return 0;
            }

            //enviar exactamente fi.size bytes, en bloques
            char block[BLOCK_SIZE];
            off_t remaining = fi.size;
            while (remaining > 0) {
                size_t to_read = remaining < (off_t)sizeof(block) ? (size_t)remaining : sizeof(block);
                ssize_t n = read(fd, block, to_read);
                if (n == -1) {
                    if (errno == EINTR) {
                        continue;
                    }
                    perror("Error al leer el archivo");
                    close(fd);
                    return 0;
                } else if (n == 0) {
                    // el archivo se redujo mientras se enviaba: el flujo quedo incompleto
                    fprintf(stderr, "El archivo cambio de tamano durante el envio\n");
                    close(fd);
                    return 0;
                }
                if (write_all(s, block, n) == -1) {
                    perror("Error al enviar el contenido");
                    close(fd);
                    return 0;
                }
                remaining -= n;
            }

            close(fd);
            return 1;
        } else {
            //no es regular entonces se envia un file_info con size = -1 para indicar que no es un archivo regular
            file_info fi;
            fi.size = -1; 
            memset(fi.filename, 0, NAME_MAX);
            strncpy(fi.filename, basename(path), NAME_MAX - 1); 

            fi.mode = buffer.st_mode;

            //enviar file_info al socket. Solo enviamos la cabezera aqui. No es necesario el resto
            int w = write_all(s, &fi, sizeof(file_info));
            if (w == -1) {
                perror("Error al enviar file_info");
                return 0;
            }

        }
    } else {
        //no existe la ruta entonces tambien se envia un file_info con size = -1 para indicar que no existe la ruta
        file_info fi;
        fi.size = -1;
        memset(fi.filename, 0, NAME_MAX);
        strncpy(fi.filename, basename(path), NAME_MAX - 1);
        fi.mode = 0; // No hay modo porque no existe el archivo
                     
        // enviar file_info al socket
        int w = write_all(s, &fi, sizeof(file_info));
            if (w == -1) {
                perror("Error al enviar file_info");
                return 0;
        }
        
        perror("La ruta no existe");
        return 0;
    }
	//1. Verificar si la ruta corresponde a un archivo regular valido
	//2. Enviar un mensaje con la informacion del archivo a transferir
	//   El otro extremo necesita conocer al menos el nombre del archivo y su tamaño
	//3. Enviar el contenido del archivo, exactamente la cantidad de bytes reportadas
	
	return 0;
}
