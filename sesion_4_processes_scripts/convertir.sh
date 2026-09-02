#!/bin/bash

## @file convertir.sh
## @brief Convierte archivos MKV a MP4 dentro de un directorio.
##
## El script busca todos los archivos con extensión .mkv en un directorio
## determinado y genera su archivo .mp4 correspondiente usando ffmpeg.
##
## Si el archivo .mp4 ya existe, la conversión se omite y se informa al usuario.
##
## El directorio puede recibirse como primer parámetro. Si no se especifica,
## se utiliza el directorio actual.
##
## Al finalizar, se informa cuántos archivos fueron convertidos y cuántos
## fueron saltados.
##
## @usage
## ./convertir.sh [directorio]
##
## @param $1 Directorio opcional donde se buscarán los archivos MKV.
##           Si no se proporciona, se utiliza el directorio actual.
##
## @return 0 Si el script finaliza correctamente.
## @return 1 Si el directorio indicado no existe o no es válido.
##
## @author Tu Nombre
## @date 2026


# Directorio donde se buscarán los archivos.
# Si no se recibe ningún parámetro, se utiliza el directorio actual.
directorio="${1:-.}"


# Verificar que la ruta recibida exista y sea un directorio.
if [ ! -d "$directorio" ]; then
    echo "Error: '$directorio' no existe o no es un directorio."
    exit 1
fi


# Contador de archivos convertidos correctamente.
convertidos=0

# Contador de archivos omitidos porque su MP4 ya existe.
saltados=0


# Cambiar al directorio indicado.
cd "$directorio" || exit 1


# Recorrer todos los archivos con extensión .mkv.
for archivo in *.mkv
do

    # Si no existen archivos MKV, el patrón '*.mkv' queda sin expandir.
    # Esta condición evita intentar procesarlo como si fuera un archivo.
    if [ ! -e "$archivo" ]; then
        continue
    fi


    # Generar el nombre del archivo MP4.
    #
    # ${archivo%.mkv} elimina el patrón '.mkv' del final del nombre.
    # Después se añade la extensión '.mp4'.
    #
    # Ejemplo:
    # pelicula.mkv -> pelicula.mp4
    #
    # El cambio de extensión se realiza mediante expansión de parámetros
    # de Bash, sin invocar programas externos.
    salida="${archivo%.mkv}.mp4"


    # Comprobar si el archivo MP4 ya existe.
    if [ -e "$salida" ]; then
        echo "Saltando '$archivo': '$salida' ya existe."

        # Incrementar contador de archivos saltados.
        ((saltados++))

        continue
    fi


    echo "Convirtiendo '$archivo' -> '$salida'"


    # Ejecutar ffmpeg para realizar la conversión de MKV a MP4.
    ffmpeg -i "$archivo" "$salida"


    # Verificar el código de salida de ffmpeg.
    if [ $? -eq 0 ]; then

        # La conversión terminó correctamente.
        ((convertidos++))

    else

        # ffmpeg produjo algún error durante la conversión.
        echo "Error al convertir '$archivo'."

    fi

done


# Mostrar resumen de ejecución.
echo ""
echo "========== RESUMEN =========="
echo "Archivos convertidos: $convertidos"
echo "Archivos saltados:    $saltados"


# Finalización correcta del script.
exit 0
