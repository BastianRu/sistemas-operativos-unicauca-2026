all: main.o
	gcc -o syscalls main.o

main.o: main.c
	gcc -c -o main.o main.c

clean:
	rm -f main.o syscalls
