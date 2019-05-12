CC      = gcc
CFLAGS  = -g
RM      = rm -f


default: all

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c 
	$(CC) $(CFLAGS) -o client client.c

clean veryclean:
	$(RM) server client
