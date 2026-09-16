# Bitacora del laboratorio - Practica 6 - Planificación de Procesos

Fecha: 2026-09-11

Punto 3: Despues de hacer a mano la planificacion de los procesos propuestos. Nos encontramos con los siguietes resultados:

- FIFO: 
    - Tiempo de espera promedio: 4.75
    - Tiempo de retorno promedio: 8.75

- SJF: 
    - Tiempo de espera promedio: 4
    - Tiempo de retorno promedio: 8

- Round Robin (Q = 2): 
    - Tiempo de espera promedio: 6.25
    - Tiempo de retorno promedio: 10.25

Por lo que podemos ver que Shortest Job First (SJF) obtuvo el menor tiempo de espera promedio de los 3.

Ahora, ¿El algoritmo se puede utilizar tal cual en un sistema operativo real? ¿Por que?

Bueno, el concepto del algoritmo es simple. "El trabajo mas corto primero", en otras palabras, el programa cuyo tiempo de ejecucion sea el menor sera el que se ejcute primero. Pero justo en esa premisa esta el problema. Como saber de antemano el tiempo de ejecucion de un programa?. O mas preciso aun, como saber de antemano el tiempo exacto que usara la CPU un programa?. Esto es imposible de adivinar si ni siquiera hemos ejecutado el programa ni una sola vez. 

Ademas, suponiendo que este tiempo se conoce, si el sistema encola muchos procesos con tiempos de CPU cortos, y esto se prolonga, se producira inanicion porque jamas habra tiempo para que los procesos largos se ejecuten. 

Punto 4: Al repetir el Round Robit con Q = 8 y Q = 1 se obtuvieron los siguientes resultados:

Q = 8
 - Tiempo de espera promedio: 4.75
 - Tiempo de retorno promedio: 8.75 

Q = 1
 - Tiempo de espera promedio: 5.5
 - Tiempo de retorno promedio: 9.5

Pero si se presta atencion al diagrama, resulta ser que con Q = 8 la ejecucion de los procesos es puramente secuencial! Mientras que con Q = 1 parece ser mas una ejecucion simultanea. Esto tiene sentido porque el Quantum es que tanto tiempo de CPU se le da a cada proceso, si es muy poco pues la CPU estara saltando entre programas cada nada, mientras que si es muy largo tendra mas tiempo para completarlos antes de saltar a otros.

Punto 6: Despues de ejecutar `while true; do :; done &` para lanzar un proceso que consuma CPU de fondo. Comprobamos con `top` que ahora el proceso que estaba de primero en la lista, con el PID proporcionado era el proceso que habiamos creado. 

Despues de eso procedimos a asignarle una amabilidad mas alta, mediante `renice -n 15 -p 3896`, lo cual hizo que su valor de CPU cambiara de vez en cuando. Lo que tiene sentido porque la amabilidad es un valor que utiliza el sistema operativo en el calculo de la prioridad de un proceso, de modo que este puede ceder la CPU si existe algun proceso de prioridad mayor. `top` reporto los valores nuevos, tanto `NI` como `PR`, `NI` de 15 como lo establecimos con el comando. Sin embargo el PID, el estado del proceso (corriendo) no cambiaron.
