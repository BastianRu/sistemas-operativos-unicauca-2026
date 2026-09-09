# Taller 5: Creacion de Procesos

## Requisitos Previos

* Compilador `gcc`
* Herramienta `make`
* Generador de documentación `Doxygen`
---

## Compilación y Ejecución

### Uso con Makefile (Recomendado)

Compilar el programa en C y generar la documentación en un solo paso:

```bash
$ make
```

### Ejecutar los programas

- Programa en C (Creación y sincronización de procesos hijos):

```bash
$ ./processes
```

- Limpiar los archivos ejecutables, objetos y la documentación generada:
```bash
$ make clean
```
- Generar manualmente la documentación del código fuente:
```bash
$ doxygen Doxyfile
```



