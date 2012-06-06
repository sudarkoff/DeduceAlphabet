CC=g++-4.7
CFLAGS=-std=c++11 -Wall -W -pedantic
main: main.o
	$(CC) $(CFLAGS) main.o

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -f main *.o
