CC = gcc
CFLAGS = -pthread -Wall
DEPS = data.h color.h
OBJ_SERVER = tcp_server.c color.c
OBJ_CLIENT = tcp_client.c color.c

all: server client

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(OBJ_CLIENT)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f server client


