CC = g++ -std=c++11 -Wall -L/usr/local/lib -I/usr/local/include
LDFLAGS = -lzmqpp -lzmq

all: server client

server: server.cpp
	$(CC) -o server.out server.cpp $(LDFLAGS)

client: client.cpp
	$(CC) -o client.out client.cpp $(LDFLAGS)

clean:
	rm -f client.out server.out
