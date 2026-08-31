# Bitacora del laboratorio - Practica 4 - Procesos, Scripts y llamadas al sistema

Fecha: 2026-08-30

Punto 6: Despues de ejecutar el proceso sleep en segundo plano por 300 segundos y localizar su pid con $ ps -ef | grep sleep cuyo resultado fue 7952, se consulta $ cat /proc/7952/status. Se detalla que los campos Name, PPid coinciden con los informados por ps, mientras que State y Threads no son informados por el mismo.

Se consulta el tamaño del archivo de las dos maneras $ stat -c %s /proc/7952/status y $ wc -c < /proc/7952/status y se oobtienen las salidas respectivamente de 0 y 1456.

Investigando sobre los comandos stat y wc, el primero consulta el estado de un archivo y entre esos atributos esta el tamaño de ese archivo. Mientras que el segundo abre un archivo y lo lee de principio a fin para contar sus palabras (o caracteres), entonces, en teoria, ambos se podrian usar para consultar el peso de un archivo. Sin embargo, el directorio /proc es especial porque el contenido de sus archivos (los procesos) como status es dinamico, no existe hasta que no se consulta o un comando requiere su contenido, y cuando el kernel lo escribe, no lo hace en disco, lo hace en RAM para mostrarlo e inmediatamente despues desaparece, entonces es lo que se llama un Archivo Virtual, y el kernel siempre reporta que el espacio ocupado por un archivo virtual es de 0 bytes, por optimizacion de recursos.

Por el contrario el comando wc realiza una llamada al sistema para abrir el archivo, y como en realidad no existe en disco, el kernel tiene que escribirlo palabra a palabra para poder entregarle esos datos a wc quien despues solicitara la lectura de ese archivo mediante llamadas de lectura sucesivas, para poder ir calculando el tamaño (contando las palabras), cuando ya no queda ninguna, wc muestra la suma final en pantalla

Punto 8: Obtuve el id del proceso padre de la shell actual usando $ echo $$, cuyo resultado fue 7865, despues ejecute recursivamente $ grep PPid /proc/[PPiD]/status donde PPid es el id padre que obtenia de cada grep previo y obtuve la siguiente cadena de resultados:

juanmunozr@DESKTOP-RM94QCM:~/os_unicauca_2026$ echo $$
7865
juanmunozr@DESKTOP-RM94QCM:~/os_unicauca_2026$ grep PPid /proc/7865/status
PPid:   7861
juanmunozr@DESKTOP-RM94QCM:~/os_unicauca_2026$ grep PPid /proc/7861/status
PPid:   7533
juanmunozr@DESKTOP-RM94QCM:~/os_unicauca_2026$ grep PPid /proc/7533/status
PPid:   7532
juanmunozr@DESKTOP-RM94QCM:~/os_unicauca_2026$ grep PPid /proc/7532/status
PPid:   7531
juanmunozr@DESKTOP-RM94QCM:~/os_unicauca_2026$ grep PPid /proc/7531/status
PPid:   2
juanmunozr@DESKTOP-RM94QCM:~/os_unicauca_2026$ grep PPid /proc/2/status
PPid:   1

Consultando los procesos con $ ps -e el proceso con el id 1 se llama systemd.
