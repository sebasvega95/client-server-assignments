CC = g++ -O2 -std=c++11 -Wall
LDFLAGS = -lzmqpp -lzmq
LOCAL_FLAGS = -I ~/zmq/include -L ~/zmq/lib

all: server client

server: server.cpp
ifeq ($(C_ENV), local)
	$(CC) -o server.o server.cpp $(LOCAL_FLAGS) $(LDFLAGS)
else
		$(CC) -o server.o server.cpp $(LDFLAGS)
endif

client: client.cpp
ifeq ($(C_ENV), local)
	$(CC) -o client.o client.cpp $(LOCAL_FLAGS) $(LDFLAGS)
else
	$(CC) -o client.o client.cpp $(LDFLAGS)
endif

clean:
	rm -f client.out server.out
