# Bitacora del laboratorio - Practica 3 - Llamadas al sistema

Fecha: 2026-08-22

Punto 1, 2 y 3:
Que intentamos: Usar write() enviando la direccion de los enteros pid/ppid para intentar mostralos por terminal

Que fallo: Se enviaron los enteros como bytes binearios a la salida estandar. el Bash los intento interpretar como caracteres ASCII y no mostro nada

Que cambiamos: Usamos la funcion sprintf para concatenar el mensaje y los numeros (pid/ppid) traducidos como caracteres ASCII

Herramientas de apoyo: Usamos un asistente para entender por que el programa no imprimia nada por terminal. Usamos man sprintf (y snprintf), man perror. 

Punto 4 y 5:
Que intentamos: Imprimir consecutivamente con printf() y write() sin salto de linea ni fflush().

Que paso: En la terminal aparecio primero el texto de write() y luego el de printf(), invirtiendo el orden del codigo.

Por que ocurrio: write() es una syscall DIRECTA al Kernel que imprime inmediatamente. printf() almacena los datos en un bufer de User Space, en el que solicita memoria dinamica (heap). Esto dado que hacer llamadas al sistema es un proceso lento y costoso, por lo que es mejor esperar para enviar un grupo grande de caracteres, a hacer una llamada por caracter.El bufer no se vacio de inmediato por la falta de '\n' (y porque aun no estaba lleno), aplazando su salida hasta la final del programa.

Herramientas de apoyo: Usamos asistentes para comprender el funcionamiento de printf() por debajo de cuerda. Tambien consultamos man fflush() para averiguar porque puede alterar el comportamiento de printf() (sin usar \n).



