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
