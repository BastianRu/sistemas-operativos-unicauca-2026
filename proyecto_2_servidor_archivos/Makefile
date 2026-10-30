CC = gcc
CFLAGS = -g -Wall -Wextra
LDFLAGS = -pthread

HEADERS = protocol.h netutil.h util.h split.h

all: server client

server: server.o util.o netutil.o protocol.o
	$(CC) $(CFLAGS) -o server server.o util.o netutil.o protocol.o $(LDFLAGS)

client: client.o util.o netutil.o protocol.o split.o
	$(CC) $(CFLAGS) -o client client.o util.o netutil.o protocol.o split.o $(LDFLAGS)

# Cada .o se recompila si cambia su .c o alguna cabecera
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

doc:
	doxygen

clean:
	-rm -rf docs *.o server client client_files files
