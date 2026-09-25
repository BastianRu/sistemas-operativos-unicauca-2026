# Bitacora del laboratorio - Proyecto 1 - Planificador de Procesos

Fecha: 2026-09-24

Punto 1. `procesar_llegadas()`:
    En esta funcion basicamente recorremos las colas actuales y empezando por la de mayor prioridad se saca cada uno de los procesos del frente de la cola. Para justo despues      pasarlos a la cola de listos. Pero la posicion en la cola de listos donde son insertadas depende de la estrategia, asi que la evaluamos:
        - Para FIFO: El if de la linea 165 no se cumple porque porque la cola es FIFO y no SJF ni SRT.
        - Para RR: El mismo if tampoco se cumple, por la misma razon.
        - Para SJF: El if se cumple y entonces se llama a la funcion `insertar_por_restante()` cuya explicacion esta mas abajo.
        - Para SRT: Lo mismo, el if se cumple y se llama a `insertar_por_restante()`. 
    
Punto 2. `insertar_por_restante()`:
    Esta era la funcion a implementar por completo. La idea es que se encuentre la posicion correcta del nuevo proceso que ingresa a la cola de listos. Tanto en SJF como SRT       se requiere que el proceso entrante vaya organizado en la cola por tiempo restante, lo que quiero decir con esto es que la expropiacion de SRT va por aparte de su inser      icon, en lo que respecta a la insercion de nuevos procesos, es lo mismo para SJF y SRT:
    - Verificamos si la cola esta vacia, en caso de estarlo pues sencillamente el proceso es unico entonces solo se mete a la cola sin mas. 
    - Recorremos toda la cola de listos desde el principio hasta el final comparando valores de p->restante, para encontrar la posicion en la que deberia ir el proceso nuevo.
    - Si esa posicion resulto ser precisamente el final de la cola, metemos el proceso al final simplemente.
    - Si no, entonces es porque esa posicion esta en algun lugar dentro de la cola excluyendo el final. En ese caso necesitamos hacerle espacio al nuevo proceso entonces comenzamos por extender la cola encolando el ultimo a una nueva posicion, y despues simplemente corremos todos los procesos una posicion hacia la derecha.
    - Con el espacio ya creado se inserta el proceso.

Punto 3. `planificar()`:
    En esta funcion se hace lo principal. Procesar el proceso, (darle CPU) y despues verificar si ese proceso vuelve o no a la cola de listos. Y de que forma. Cuando el proceso no termino en su quantum (actual->restante != 0) entra un switch por estrategia que es literalmente donde cada algoritmo se distingue de los demas:
    - FIFO: `cola.listos.push_front(actual)`. El proceso vuelve pero al frente, no al final. Como no hay nadie que se le pueda adelantar, el proceso recupera la cpu de inmediato en el siguiente ciclo, asi que aunque el quantum sea de 2 y la rafaga de 7, el resultado es el mismo que si no hubiera limite: el proceso se ejecuta de corrido (se nota en la secuencia como varios bloques del mismo proceso seguidos).
    - SJF: tambien usa push_front, por la misma razon que FIFO: SJF no expropia. La diferencia entre FIFO y SJF entonces no esta en esta linea sino en el punto 1, en como se ordenan las llegadas.
    - RR: `cola.listos.push_back(actual)`. Aqui si hay diferencia real, el proceso se va al final de la cola en vez de al frente, entonces le toca esperar su turno otra vez como cualquier otro. Esta es la linea que hace que RR reparta la cpu en vueltas en lugar de dejarla monopolizada.
    - SRT: en vez de push_front o push_back se llama de nuevo a `insertar_por_restante()`. El proceso que no termino no se va ni al frente ni al final fijo, se acomoda segun cuanto le quede, para que en la siguiente vuelta se le pueda volver a comparar contra los demas.

Punto 4. Expropiacion de SRT (dentro de `planificar()`, lineas 223 a 235):
    Este es el bloque que le falta a SJF para ser SRT. Antes de dejar correr al proceso todo su quantum, si la cola es SRT se recorren las llegadas que van a pasar durante ese quantum y se calcula cuanto le quedaria al proceso actual en el instante en que llega el nuevo (`restante_actual`). Si la rafaga del que llega es menor que eso, entonces:
    - se recorta `quantum_asignado` para que el actual solo reciba cpu hasta el instante en que llega el otro proceso.
    - se pone `cambiar_de_cola = false` para que en la siguiente vuelta del while se siga evaluando la misma cola (no se salta a la de al lado), porque lo que expropio a `actual` fue una llegada, no que se le acabaran los procesos a su cola.
    Como el proceso expropiado no termino, sigue el flujo normal y cae en el switch del punto 3, entrando por el case SRT, entonces vuelve a la cola ordenado por lo que le quedaba.

Punto 5. Caso propio con tres colas de prioridad (`test/caso_propio.txt`):
    Armamos un caso con 7 procesos repartidos en tres colas, cada una con un algoritmo distinto:
        - Cola 1 (alta prioridad): FIFO, quantum 1. Procesos P1 (llega 0, rafaga 5) y P2 (llega 3, rafaga 3).
        - Cola 2 (prioridad media): SRT, quantum 2. Procesos P3 (llega 0, rafaga 6), P4 (llega 2, rafaga 2) y P5 (llega 4, rafaga 4).
        - Cola 3 (baja prioridad): RR, quantum 3. Procesos P6 (llega 0, rafaga 7) y P7 (llega 5, rafaga 3).
    Resultado: tiempo total 30, espera promedio 13.429.

    Para comparar, `test/caso_propio_una_cola.txt` mete los mismos 7 procesos en una unica cola FIFO
    (mismas llegadas y rafagas, sin prioridades). Resultado: tiempo total 30 (el trabajo total no
    cambia), pero espera promedio 12.857 en vez de 13.429, y la espera de cada proceso individual
    cambia por completo entre un caso y otro (por ejemplo P1 pasa de esperar 19 a esperar 0, y P4 pasa
    de esperar 16 a esperar 0).

    Por que cambia el resultado: en una sola cola FIFO el unico criterio es el orden de llegada, y quien
    llega primero conserva la CPU hasta terminar, sin importar cuanto dure su rafaga; por eso P4, que
    llega en el instante 2 con una rafaga corta de 2, espera a que terminen P1 y P3 antes de que le
    toque. Con tres colas, en cambio, `planificar()` reparte turnos de forma circular entre colas
    (lineas 295 a 326), del tamano del quantum de cada una, de modo que ninguna cola monopoliza la CPU
    aunque su algoritmo interno no expropie: P1, pese a estar en la cola de mayor prioridad, solo recibe
    su quantum de 1 unidad cada vez que le toca su turno, y ese turno se reparte con las otras dos
    colas, por lo que termina mucho mas tarde que en el caso de una sola cola. Ademas, dentro de la
    cola 2 el algoritmo SRT ordena a los procesos solo entre quienes comparten esa prioridad, asi que
    P4 pasa a ser el mas corto de su cola y se ejecuta casi de inmediato. En resumen, repartir en colas
    cambia el resultado porque agrega un limite a cuanta CPU seguida puede recibir un proceso (el turno
    circular entre colas) y porque cada algoritmo compite solo dentro de su propio grupo, mientras que
    una sola cola deja todo sujeto a un unico criterio para todos los procesos por igual.


