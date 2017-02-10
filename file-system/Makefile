CC = g++ -std=c++11 -Wall
LDFLAGS = -lzmqpp -lzmq

all: server client

server: server.cpp
	$(CC) -o server.o server.cpp $(LDFLAGS)

client: client.cpp
	$(CC) -o client.o client.cpp $(LDFLAGS)

clean:
	rm -f client.out server.out
