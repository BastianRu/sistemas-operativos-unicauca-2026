#!/bin/bash

if [ -z "$1" ]
then
	echo "El identificador del proceso que ejecuta este script es $$"
else
	if [ ! -d "/proc/$1" ]; then
 		   echo "El proceso con PID $1 no existe o ya no esta vivo."
	           exit 2
	fi
	nombre_crudo=$(grep Name /proc/$1/status)
	estado_crudo=$(grep State /proc/$1/status)
	ppid_crudo=$(grep PPid /proc/$1/status)
	threads_crudo=$(grep Threads /proc/$1/status)
	nombre=${nombre_crudo#Name:[[:space:]]}
	estado=${estado_crudo#State:[[:space:]]}
	ppid=${ppid_crudo#PPid:[[:space:]]}
	hilos=${threads_crudo#Threads:[[:space:]]}
	echo "El nombre de este programa es $nombre"
	echo "El estado de este programa es $estado"
	echo "El padre de este programa es $ppid"
	echo "La cantidad de hilos de este programa es $hilos"
	exit 0
fi

