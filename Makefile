CC=g++
CFLAGS=-Wall -W -pedantic
main: main.o

clean:
	rm -f main *.o
