# Makefile
FLAGS = -std=c99 -Wall -O1 -pthread

all: server client

client: client.c
	gcc ${FLAGS} client.c -o client

server: server.c bounded.c wrapper.c
	gcc ${FLAGS} bounded.c wrapper.c server.c -o server

clean:
	rm -f client server
