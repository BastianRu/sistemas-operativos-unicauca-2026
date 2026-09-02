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

Punto 22: Despues de ejecutar el script y el programa infoproc, para el mismo programa, se comprobo que efectivamente las salidas de ambos entregan datos identicos. Sin embargo el proceso por el cual los obtienen es muy distinto. Incluso cuando la informacion se termina obteniendo del mismo archivo virtual en /proc.

En el script de bash, se obtiene la informacion del archivo usando el comando grep que a fin de cuentas es un programa en C, que investigando, usa fork() para crear un proceso hijo y luego ese proceso hijo sobreescribe su propio codigo para correr el comando cat (que es para leer archivos enteros), proceso que se logra ejecutando el codigo del propio cat, guardado en /bin/cat, de manera que lee todo el archivo de status en /proc, despues se redirige su salida (que por defecto seria stdout a la pantalla) al padre, y este se encarga de procesar todo el archivo para entregar el patron solicitado. Despues se procesan los resultados de grep para cada caso con manipulacion de cadenas en bash. Finalmente se muestran por stdout con echo.

En el programa en C, se usa open, read y close, que son llamadas al sistema, se solicita la lectura de /proc directamente al kernel, sin librerias de por medio, los datos que el kernel va entregando poco a poco al buffer en la RAM del programa. Despues se procesa la informacion del buffer manualmente para extraer la informacion que se busca y finalmente se muestra en pantalla. Entonces el programa en C es bastante mas directo que el script, porque el ejecutable interactua directamente con el kernel mediante llamadas al sistema.

Punto 23: Corri $ python3 -c "import time; [time.sleep(0.05) or sum(range(1000000)) for _ in iter(int, 1)]" & 
para obtener un proceso que altere su estado, su indentificador para el caso fue 16468 y procedi a ejecutar ./infoproc y ./infoproc.sh cuyas respuestas variaron de la una a la otra:

Al usar un binario unico (como Python) en lugar de una subshell de Bash, /proc reporta directamente el nombre delejecutable activo y no bash. El proceso alterna entre running y suspension muchas veces por segundo. Dado que el Kernel genera el /proc status en tiempo real al leerlo, la minima diferencia de tiempo entre ambas ejecuciones hace que capturen estados distintos, esto se acentua porque el programa en C lee de forma instantanea mediante la llamada directa read(), mientras que el script en Bash añade algo de latencia al crear subprocesos intermediarios.

Punto 24: Desde que presionas enter hasta que reaparece el prompt, el shell lee y analiza la cadena ingresada mediante read(), invoca la llamada al sistema fork() para duplicarse y crear un proceso hijo, y dentro de este hijo utiliza execve() para sobreescribirse con el programa solicitado (como en el grep). Mientras el ejecutable corre,el shell padre suspende su ejecucion bloqueandose en waitpid() a la espera de que el hijo finalice, una vez el kernel le avisa la terminacion, el shell recupera el control, toma el codigo de salida e imprime nuevamente el simbolo de espera en pantalla.
