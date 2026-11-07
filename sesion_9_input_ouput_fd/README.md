# Entrada y Salida con descriptores

## Integrantes

- Juan Sebastian Muñoz Ruiz: <juanmunozr@unicauca.edu.co>
- Juan Felipe Gallardo Orozco: <juangallardo@unicauca.edu.co>  

## Planteamiento del taller
El sistema operativo abstrae las operaciones de entrada y salida mediante
descriptores de archivo. Un descriptor de archivo es un número entero que
identifica un recurso abierto, que a su vez puede representar un archivo, una
tubería o un socket. Al iniciar el proceso, el sistema operativo automáticamente
asigna los descriptores para la entrada estándar, la salida estándar y el error
estándar, que corresponden a los números 0, 1 y 2 respectivamente.
Los descriptores de archivo se usan en las llamadas al sistema read y write
para leer y escribir datos. Cuando un descriptor de archivo se abre, el sistema
operativo mantiene un apuntador interno que indica la posición actual en el
archivo. Cada vez que se realiza una operación de lectura o escritura, este
apuntador se actualiza automáticamente. Este apuntador se puede mover con
la llamada al sistema lseek.

La lectura y escritura de archivos se realiza en bloques de bytes, y el tamaño
del bloque puede variar según el sistema operativo y la implementación de la
biblioteca estándar. Normalmente, el programador usa la constante BUFSIZ
(definida en stdio.h) para determinar el tamaño del bloque de lectura y
escritura.

Sin embargo, en este taller se puede usar un tamaño de bloque definido por el
programador.

Existe otra familia de funciones para leer y escribir archivos, que son las funcio-
nes de flujo de la biblioteca estándar: fopen, fread, fwrite y fclose. Estas funciones usan internamente descriptores de archivo, pero además implementan un buffer interno que permite optimizar la lectura y escritura de datos usando RAM como buffer.

Las llamadas al sistema read y write son más lentas que las funciones de flujo,
porque cada llamada implica un cambio de contexto entre el espacio de usuario
y el espacio del kernel. Por eso, cuando se leen o escriben muchos bytes, es más
eficiente usar un buffer grande para reducir la cantidad de llamadas al sistema.


## Enunciado

1. Escribir un programa que copie un archivo usando solo open, read, write
y close. El nombre del archivo de origen y el de destino se reciben como
argumentos de línea de comandos.

2. Tratar correctamente el caso en que read devuelve menos bytes de los
pedidos. En este caso se debe escribir solo lo leido, no el buffer completo.

3. Verificar el resultado comparando origen y destino, y probarlo también
con un archivo binario, no solo de texto:
$ ./copiar origen.bin destino.bin
$ cmp origen.bin destino.bin && echo iguales

4. Medir el tiempo de la copia de un archivo grande con tres tamaños de
buffer: 1 byte, 512 bytes y 4096 bytes.
$ time ./copiar grande.dat copia.dat

5. Explicar en la bitácora a qué se debe la diferencia. La respuesta tiene que
ver con cuántas veces se entra al núcleo.

6. Escribir la misma copia usando fopen, fread, fwrite y fclose, y comparar el tiempo con el caso de 1 byte. Explicar por qué la diferencia es menor de lo esperado.
