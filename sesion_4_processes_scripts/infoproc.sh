#!/bin/bash

pid=${1:-$$}

if [ ! -d "/proc/$pid" ]; then
    echo "El proceso con PID $pid no existe o ya no esta vivo."
    exit 2
fi

nombre_crudo=$(grep "^Name:" /proc/$pid/status)
estado_crudo=$(grep "^State:" /proc/$pid/status)
ppid_crudo=$(grep "^PPid:" /proc/$pid/status)
threads_crudo=$(grep "^Threads:" /proc/$pid/status)

nombre=${nombre_crudo##*:*[[:space:]]}
estado=${estado_crudo##*:*[[:space:]]}
ppid=${ppid_crudo##*:*[[:space:]]}
hilos=${threads_crudo##*:*[[:space:]]}

echo "El nombre de este programa es $nombre"
echo "El estado de este programa es $estado"
echo "El padre de este programa es $ppid"
echo "La cantidad de hilos de este programa es $hilos"

while [ "$ppid" != "1" ] && [ -n "$ppid" ]
do
    echo "Pid del programa actual $pid - Padre del programa: $ppid"

    pid=$ppid
    
    ppid_crudo=$(grep "^PPid:" /proc/$pid/status)
    ppid=${ppid_crudo##*:*[[:space:]]}
done

echo "Pid del programa actual $pid - Padre del programa: $ppid"

exit 0
