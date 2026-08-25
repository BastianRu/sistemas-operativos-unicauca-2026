# Bitacora del laboratorio - Practica 3 - Llamadas al sistema

Fecha: 2026-22-08

Que intente: Usar write() enviando la direccion de los enteros pid/ppid para intentar mostralos por terminal
Que falló: Se enviaron los enteros como bytes binearios a la salida estandar. el Bash los intento interpretar como caracteres ASCII y no mostro nada
Que cambie: Use la funcion sprintf para concatenar el mensaje y los numeros (pid/ppid) traducidos como caracteres ASCII
Herramientas de apoyo: Use un asistente para entender por que el programa no imprimia nada por terminal. Use man sprintf (y snprintf), man perror. 



