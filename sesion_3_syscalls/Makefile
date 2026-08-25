all: syscalls syscalls2

syscalls: main.o
	gcc -o syscalls main.o

main.o: main.c
	gcc -c -o main.o main.c -Wall

syscalls2: main2.o
	gcc -o syscalls2 main2.o

main2.o: main2.c
	gcc -c -o main2.o main2.c -Wall

docs:
	doxygen Doxyfile

clean:
	rm -f main.o main2.o syscalls syscalls2
	rm -rf docs