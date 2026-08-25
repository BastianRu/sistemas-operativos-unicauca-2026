# Llamadas al sistema

## Integrantes

- Juan Sebastian Muñoz Ruiz: <juanmunozr@unicauca.edu.co>
- Juan Felipe Gallardo Orozco: <juanfelipegallardoorozco@gmail.com>  
- Juan Pablo Medina Bolanios <juanmedinatm@unicauca.edu.co>

## Planteamiento del problema

### Enunciado

1. Escribir un programa en C que:
- Obtenga su identificador de proceso y el de su proceso padre.
- Escriba un mensaje en la salida estándar usando `write`, no `printf`.
- Termine con un código de salida distinto de cero, elegido por el autor.

2. Verificar el valor de retorno de cada llamada al sistema. Si una falla,
informar la causa con `perror` y terminar con un código de error.

3. Comprobar el código de salida del programa desde la terminal:

```bash
$ ./programa
$ echo ?
```

4. Escribir un segundo programa que imprima el mismo texto dos veces: una
con `printf` y otra con `write`, en ese orden, sin usar `fflush` ni terminar.

5. Explicar en la bitácora por qué aparecen en ese orden. La respuesta tiene
que ver con dónde queda el texto antes de llegar a la pantalla.

## Requerimentos previos

- Instalar las dependencias necesarias:
```bash
$ sudo apt install gcc make doxygen
```

## Ejecución

### 1. Compilar los programas

Desde la carpeta del proyecto, compila ambos ejecutables con:

```bash
$ make
```

Esto genera dos binarios: `syscalls` (puntos 1-3) y `syscalls2` (punto 4).

### 2. Ejecutar el primer programa (obtención de PID/PPID)

```bash
$ ./syscalls
$ echo $?
```

El programa imprime el PID y PPID del proceso mediante `write()`, y termina con el código de salida `73`, visible con `echo $?`.

### 3. Ejecutar el segundo programa (orden de printf vs write)

```bash
$ ./syscalls2
```

Para comparar el orden de escritura entre una terminal y un archivo:

```bash
$ ./syscalls2 > salida.txt
$ cat salida.txt
```

### 4. Generar la documentación con Doxygen

```bash
$ make docs
```

La documentación se genera en `docs/html/index.html`, navegable desde cualquier navegador:

```bash
$ xdg-open docs/html/index.html
```

### 5. Limpiar los archivos generados

```bash
$ make clean
```

Elimina los archivos objeto (`.o`), los ejecutables generados y la documentacion en Doxygen(archivo docs).
