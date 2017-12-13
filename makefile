CC=g++

all: main

main: main.cpp
	$(CC) main.cpp -lpam -lpam_misc -o server.o

clean: 
	rm -rf server.o

