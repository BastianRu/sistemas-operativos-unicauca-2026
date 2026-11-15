# Bitacora del laboratorio - Sesion 10 - Sistema de archivos

Fecha: 2026-11-14

Primera parte: Observar el Sistema

1. Al consultar los sistemas de archivos montados en mi sistema, obtuve:

Filesystem      Size  Used Avail Use% Mounted on
tmpfs           3.2G  3.2M  3.2G   1% /run
/dev/nvme0n1p6  449G   75G  352G  18% /
tmpfs            16G  7.1M   16G   1% /dev/shm
tmpfs           5.0M   12K  5.0M   1% /run/lock
efivarfs        128K   17K  107K  14% /sys/firmware/efi/efivars
/dev/nvme0n1p5  8.0G   24K  7.6G   1% /mnt/datos
/dev/nvme0n1p1   96M   38M   59M  40% /boot/efi
tmpfs           3.2G   22M  3.2G   1% /run/user/1000

Indagando sobre la informacion que muestra el comando, la salida muestra como tengo la particion principal de mi Linux en /dev/nvme0n1p6 con un espacio disponible de 352GB, usado de 75GB y total de 449GB. La carpeta asociada con la que se accede es el root, es decir / como se esperaria. Los sistemas de tmpfs al parecer son sistemas de archivos que usa la memoria RAM de manera exclusiva, y los gestiona ella misma. efivarfs es una interfaz interna del kernel para comunicarse con la configuración de la BIOS/UEFI de la placa base. La mia es una B550 de Gigabyte entonces deberia ser de UEFI. La particion /dev/nvme0n1p5 es una particion que cree hace tiempo, para experimentar, no la he usado por eso esta vacia. Y /dev/nvme0n1p1 es la particion en la que se almacena todo lo del arranque del sistema, porque reconozco el directorio /boot/efi de las clases de teoria.

Al correr el segundo comando, `df -i` obtuve:

Filesystem       Inodes   IUsed    IFree IUse% Mounted on
tmpfs           4097690    1742  4095948    1% /run
/dev/nvme0n1p6 29941760 1112442 28829318    4% /
tmpfs           4097690      50  4097640    1% /dev/shm
tmpfs           4097690       5  4097685    1% /run/lock
efivarfs              0       0        0     - /sys/firmware/efi/efivars
/dev/nvme0n1p5   535392      11   535381    1% /mnt/datos
/dev/nvme0n1p1        0       0        0     - /boot/efi
tmpfs            819538     194   819344    1% /run/user/1000

Ahora las columnas ya no muestran espacio si no informacion sobre inodos. Investigando me encuentro con que los inodos resultan ser estructuras de datos especiales del sistema operativo que guardan cierta informacion sobre los archivos y carpetas como los permisos, dueños, fechas y ubicacion, pero curiosamente no el nombre (tampoco el contenido pero pues es muy obvio el porque no), porque el inodo almacena los metadatos al archivo, y en Linux varios nombres pueden referenciar un mismo inodo, por lo que si lo guardara eso seria imposible. 

Ahora bien, de antemano me causo curiosidad por que existen un numero limitado de inodes y que relacion tiene el numero con el espacio en disco.
Indagando en la historia de los sistemas de archivos. Resulta ser que la estrategia que usan para facilitar la busqueda de un archivo en concreto, cuyo contenido sabemos que esta en alguna parte del disco, y quiza fragmentado, es asginar a cada contenido de archivo (o bloque), es decir los bytes reales, una estructura de datos que aporte la informacion de donde encontrarlo, el inode cumple esta funcion. Y existe una tabla ordenada de nodos, en la que cada elemento es facilmente ubicable mediante una operacion matematica simple. Ahora bien, cada nodo tiene un peso, por defecto 256 bytes, almacenados tambien en disco. Ahora, un nodo nuevo no se crea en el momento en el que se crea un archivo nuevo, el archivo se crea, y se le asigna un nodo ya existente, entonces surge la pregunta: Cuantos nodos existentes hay? Y por que hay los que hay? 

Cuando se particiona un disco se crea un numero de inodos para el sistema de archivos de esa particion. La convencion y lo que mejor funciona para la mayoria de usuarios es: por cada 16KB de espacio en disco, se crea un nodo, si mi maquina por ejemplo tiene 1TB de almacenamiento, se crean 67,108,864 nodos cuando se formatea el disco. Es un numero al que se ha llegado de manera practica. Si quisieramos tener mas nodos disponibles para usar, tendriamos que reducir los bytes por nodo, por ejemplo si por cada 1KB de espacio crearamos 1 nodo, en un disco de 1TB, tan solo el espacio requerido para almacenar todos los nodos seria de 256 GB, es decir un cuarto de disco quedaria inutilizable para guardar contenido desde el principio. Con 16KB solo son ~16GB. 

Entonces la respuesta a la pregunta es clara: Si mi sistema se queda sin inodos disponibles para asignar a archivos, no voy a poder guardar mas archivos. Incluso si el disco tiene espacio sin usar. 




