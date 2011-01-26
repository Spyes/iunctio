CC = gcc
CFLAGS = -pthread
LFLAGS = -lncurses -pthread
CSRC = client.c window.c
SOBJ = server.o window.o

client:
	$(CC) $(CSRC) -o client $(LFLAGS)

client.o: client.c
	$(CC) client.c -o client.o -c $(CFLAGS)

server.o: server.c
	$(CC) server.c -o server.o -c $(CFLAGS)

window.o: window.c
	$(CC) window.c -o window.o -c $(CFLAGS)

server: $(SOBJ)
	$(CC) $(SOBJ) -o server $(LFLAGS)

cclient:
	rm -f client

cserver:
	rm -f server $(SOBJ)

