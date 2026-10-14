# Bitacora del laboratorio - Practica 8 - Señales en un servidor concurrente

Fecha: 2026-10-13

## Punto 1: Ctrl-C sobre el servidor sin modificaciones

**Que se hizo:** Se ejecutó `./servidor_tablero rwlock`, se conecto un cliente escritor
(`./cliente_tablero 127.0.0.1 escritor A 50`) y, tras aproximadamente un segundo, se envio
`SIGINT` al servidor (`Ctrl-C`).

**Que pasa con el servidor al terminar:** Termina de forma abrupta e inmediata. No imprime
ningún mensaje ni contadores, y el codigo de salida es 130 (128 + 2, el número de `SIGINT`),
lo que indica que el proceso murio por la señal. El programa no tiene manejador para
`SIGINT`, así que se aplica la acción por defecto del sistema operativo, que es terminar
el proceso. El socket de escucha tampoco se cierra de buena manera que seria con `close`, lo
cierra el kernel al destruir el proceso.

**Que ocurre con el cliente conectado:** Al morir el servidor, el kernel cierra todos sus
descriptores, incluido el socket de esa conexion, y el cliente detecta que su extremo
quedo cerrado. En la prueba, el cliente informo
`La conexion se interrumpio en la repeticion 8` tras completar 7 escrituras, y termino
con normalidad (codigo de salida 0) porque su propio codigo si valida que pasa si eso sucede entonces el cliente no
se cae, lo que recibe es el fin de la conexion.

## Punto 4: Proteccion de los contadores

Para los dos contadores se eligio `atomic_int` de `<stdatomic.h>`, incrementado con `atomic_fetch_add`.
El criterio fue que cada contador es una sola variable y lo unico que se hace es sumarle uno, asi que
una operacion atomica es suficiente: no bloquea a los hilos y no hay candado que se pueda quedar tomado.
Un mutex tambien serviria, pero costaria mas y harian fila las lecturas que `rwlock` deja correr en
paralelo. Sin proteccion `contador++` son tres pasos (leer, sumar, escribir) y dos hilos se pisarian
perdiendo cuentas, igual que le paso al tablero en la practica anterior. Con 4 lectores de 5 lecturas y
2 escritores de 10 escrituras el servidor reporto 20 lecturas y 20 escrituras, justo lo esperado.

## Punto 6: Envio de señales

- `kill -ALRM $!`: el servidor presento el informe del tablero y siguio funcionando.
- `kill -INT $!`: el servidor salio del ciclo, cerro el socket de escucha, presento los contadores y
  termino con codigo 0.
- `kill -KILL $!`: el servidor murio al instante (codigo 137) sin ejecutar nada de su codigo.

Al intentar capturar `SIGKILL` con `sigaction` el sistema devuelve `Invalid argument`, no deja ni
capturarla ni ignorarla. Esto beneficia al sistema porque siempre debe existir una forma segura de
matar un proceso: si se pudiera capturar, un programa con errores (o malicioso) podria ignorarla y el
administrador no tendria como detenerlo.

## Punto 7: Fallo provocado

Al arrancar un segundo servidor mientras el primero escuchaba, `bind` fallo y `perror` mostro:

    bind: Address already in use

El programa termina con `EXIT_FAILURE`. `SO_REUSEADDR` no lo evita porque solo permite reutilizar
un puerto que quedo en `TIME_WAIT`, no uno que otro socket tiene en escucha.

## Punto 8: signal(SIGPIPE, SIG_IGN)

Esa linea hace que el servidor ignore `SIGPIPE`. Sin ella, cuando un cliente cierra la conexion de golpe
y el servidor intenta hacer `write` sobre ese socket, el kernel le envia `SIGPIPE` y la accion por
defecto es terminar el proceso, o sea que el servidor entero se caeria junto con todos los demas clientes
por culpa de uno solo. Con la linea, `write` simplemente devuelve -1 con `EPIPE` y el hilo de ese cliente
cierra su conexion y termina.

## Informe periodico

La alarma se programa cada 5 segundos (o se dispara a mano con `kill -ALRM`). El primer informe salio por `kill -ALRM` en plena carga y el segundo por la alarma periodica. Salida obtenida:

    Tablero de 64 casillas, politica rwlock, escuchando en el puerto 5500
    [informe] version 7 tablero AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    [informe] version 20 tablero BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
    Servidor terminado: 20 lecturas y 20 escrituras atendidas
