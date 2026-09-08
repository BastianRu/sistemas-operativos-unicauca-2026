# Bitacora del laboratorio - Practica 5 - Creacion de procesos

Fecha: 2026-09-06

Punto 3: Despues de añadir el codigo despues de la llamada a 'execvp' se comprobo que no se ejecuta normalmente. Esto se debe a que si execvp logra su ejecucion va a reemplazar por completo la memoria delproceso hijo por el del codigo del ejecutable, para este caso 'factor', esto hace que sea virtualmenteimposible ejecutar el codigo que antes estaba ahi, porque ahora ya ni siquiera existe en la memoria del nuevo programa. Sin embargo si la llamada a execvp falla, por ejemplo al no encontrar el ejecutable que se esta pidiendo o que el mismo devuelva un error. Entonces excvp devolvera una excepcion y por ende el programa hijo seguira su codigo con normalidad.

 
