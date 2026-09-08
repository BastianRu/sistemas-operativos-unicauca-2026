all: processes 

processes: main.o
	gcc -o processes main.o

main.o: main.c
	gcc -c -o main.o main.c -Wall

clean:
	rm -f main.o processes 
