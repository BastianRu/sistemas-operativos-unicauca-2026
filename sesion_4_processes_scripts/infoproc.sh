#!/bin/bash

##
# @file infoproc.sh
# @brief Script para consultar la información y jerarquía de un proceso en /proc.
# @details Extrae Name, State, PPid y Threads del archivo /proc/[PID]/status usando
#          grep y limpieza mediante Parameter Expansion, iterando luego en la
#          árbol de procesos hasta alcanzar el proceso raíz (PID 1).
# @author Tu Nombre
#

##
# @brief PID objetivo obtenido desde el primer argumento o mediante el PID del shell ($$).
#
pid=${1:-$$}

# Validación de la existencia del proceso en procfs
if [ ! -d "/proc/$pid" ]; then
    echo "El proceso con PID $pid no existe o ya no esta vivo."
    exit 2
fi

##
# @brief Lectura de las líneas crudas del archivo /proc/[PID]/status.
#
nombre_crudo=$(grep "^Name:" /proc/$pid/status)
estado_crudo=$(grep "^State:" /proc/$pid/status)
ppid_crudo=$(grep "^PPid:" /proc/$pid/status)
threads_crudo=$(grep "^Threads:" /proc/$pid/status)

##
# @brief Extracción de valores limpios mediante Parameter Expansion (${var##*:*[[:space:]]}).
#
nombre=${nombre_crudo##*:*[[:space:]]}
estado=${estado_crudo##*:*[[:space:]]}
ppid=${ppid_crudo##*:*[[:space:]]}
hilos=${threads_crudo##*:*[[:space:]]}

# Imprimir información básica del proceso
echo "El nombre de este programa es $nombre"
echo "El estado de este programa es $estado"
echo "El padre de este programa es $ppid"
echo "La cantidad de hilos de este programa es $hilos"

##
# @brief Bucle de rastreo de jerarquía de procesos.
# @details Sube cíclicamente por el árbol de ancestros leyendo el PPid de cada padre
#          hasta llegar al proceso con PID 1 (init/systemd).
#
while [ "$ppid" != "1" ] && [ -n "$ppid" ]
do
    echo "Pid del programa actual $pid - Padre del programa: $ppid"

    pid=$ppid
    
    ppid_crudo=$(grep "^PPid:" /proc/$pid/status)
    ppid=${ppid_crudo##*:*[[:space:]]}
done

# Impresión del último nivel (PID 1)
echo "Pid del programa actual $pid - Padre del programa: $ppid"

exit 0
