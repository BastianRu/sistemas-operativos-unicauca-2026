# Makefile del tablero compartido
#
# make          compila el servidor y el cliente
# make docs     genera la documentacion con Doxygen en docs/
# make clean    elimina ejecutables, resultados de carga.sh y docs/

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -pthread

PROGRAMAS = servidor_tablero cliente_tablero

all: $(PROGRAMAS)

servidor_tablero: servidor_tablero.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

cliente_tablero: cliente_tablero.c
	$(CC) $(CFLAGS) -o $@ $<

docs:
	doxygen Doxyfile

clean:
	rm -f $(PROGRAMAS) lector_*.txt escritor_*.txt
	rm -rf docs

.PHONY: all docs clean
