CC=g++

all: main client

main: main.cpp
	$(CC) main.cpp -lpam -lpam_misc -o server.o

client: client.cpp
	$(CC) client.cpp -o client.o

clean: 
	rm -rf server.o client.o

