## Planteamiento del problema

Se suponen conocidos el concepto de archivo, la estructura de directorios, los metadatos y los permisos. La idea que ordena todo el taller: el nombre no es el archivo. El archivo es la estructura de metadatos que apunta a los bloques de datos, y el directorio solo asocia un nombre con ella.

### Primera parte: observar el sistema'

1. Consultar los sistemas de archivos montados, su tipo y su espacio libre:

```bash
$ df -h
$ df -i
$ lsblk -f
```

2. Explicar en la bitácora qué informa df -i y en qué situación un sistema
de archivos puede quedar inutilizable aunque df -h muestre espacio
libre.

3. Consultar todos los metadatos de un archivo y de su directorio contenedor:
$ stat archivo.txt

```bash
$ ls -li
```

### Segunda parte: enlaces

4. Crear un enlace duro y un enlace simbólico a un mismo archivo:

```bash
$ ln archivo.txt duro.txt
$ ln -s archivo.txt simbolico.txt
$ ls -li archivo.txt duro.txt simbolico.txt
```
5. Comparar el identificador de la estructura de metadatos y el contador de
enlaces en los tres casos. Anotar los valores.

6. Borrar el archivo original y comprobar qué ocurre con cada enlace. Expli-
car el resultado.

7. Responder: ¿por qué no se puede crear un enlace duro a un archivo
que está en otro sistema de archivos? ¿Por qué el enlace simbólico sí lo
permite?

### Tercera parte: desde un programa

8. Escribir un programa que reciba el nombre de un archivo como argumento
e informe su tamaño, su tipo, sus permisos, su contador de enlaces y su
fecha de última modificación.

9. Probarlo sobre un archivo normal, sobre un directorio y sobre un enlace
simbólico. Comentar las diferencias.
