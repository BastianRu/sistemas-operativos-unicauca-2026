# Taller 8: Señales en un servidor concurrente

Este proyecto extiende el servidor TCP del tablero compartido (`servidor_tablero.c`) para que maneje
señales: `SIGINT` termina el servidor de forma ordenada y `SIGALRM` produce un informe periódico del
tablero. Los contadores de órdenes atendidas se protegen con operaciones atómicas. El cliente
(`cliente_tablero.c`) y el script `carga.sh` generan la carga para probarlo.

## Requisitos Previos

- Compilador `gcc`, la herramienta `make` y `Doxygen`.

---

## Compilación y Ejecución

### Uso con Makefile (Recomendado)

Compilar el servidor y el cliente:
```bash
$ make
```

Generar la documentación (queda en `docs/`):
```bash
$ make docs
```

## Ejecutar los programas

- Servidor (política de sincronización: `sin`, `mutex` o `rwlock`):
```bash
$ ./servidor_tablero rwlock
```

- Cliente (rol `lector` o `escritor`, caracter y número de repeticiones):
```bash
$ ./cliente_tablero 127.0.0.1 escritor A 50
```

- Carga de varios lectores y dos escritores a la vez:
```bash
$ bash carga.sh [direccion_ip] [lectores] [lecturas_por_lector]
```

## Señales del servidor

Desde otra terminal:
```bash
$ kill -ALRM <PID>   # presenta la versión y el contenido del tablero
$ kill -INT <PID>    # cierra el socket de escucha, presenta los contadores y termina
$ kill -KILL <PID>   # no se puede capturar: el proceso muere de inmediato
```
Además, el informe del tablero se presenta solo cada 5 segundos.

- Limpieza (ejecutables, resultados de `carga.sh` y `docs/`):
```bash
$ make clean
```

La bitácora con las respuestas de la práctica está en `bitacora.md`.
