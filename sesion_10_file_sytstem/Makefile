# Makefile de informar
#
# make          compila informar y genera la documentacion en docs/
# make docs     solo genera la documentacion con Doxygen
# make clean    elimina el ejecutable y docs/

CC = gcc
CFLAGS = -Wall -Wextra -O2

all: informar docs

informar: informar.c
	$(CC) $(CFLAGS) -o $@ $<

docs:
	doxygen Doxyfile

clean:
	rm -f informar
	rm -rf docs

.PHONY: all docs clean
