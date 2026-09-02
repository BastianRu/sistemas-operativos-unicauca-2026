# Taller 4: Procesos, Scripts y Llamadas al Sistema

Este proyecto contiene la implementación en C (`infoproc.c`) y Bash (`infoproc.sh`) para consultar metricas e informacion sobre procesos desde el sistema de archivos virtual `/proc`.

## Requisitos Previos

- Compilador `gcc` y la herramienta `make` ademas de `Doxygen`.

---

## Compilación y Ejecución

### Uso con Makefile (Recomendado)

Compilar el programa en C y generar la documentación en un solo paso:
```bash
$ make
```
## Ejecutar los programas

- Programa en C (PID actual o especificando PID)
```bash
$ ./infoproc
$ ./infoproc <PID>
```
- Script en Bash (PID actual o especificando PID)
```bash
$ ./infoproc.sh
$ ./infoproc.sh <PID>
```

-  Script de conversion de archivos MKV
```bash
$ ./convertir.sh
$ ./convertir.sh <DIR>
```

- Limpieza
```bash
$ make clean
```
- Documentacion con Doxygen
```bash
$ doxygen Doxyfile
```
