CC = gcc
CFLAGS = -g
LIBS = -lm

all: main

main: main.o id3.o
	$(CC) $^ -o $@ $(LIBS)

main.o: main.c id3.h

id3.o: id3.c id3.h

clean:
	rm main main.o id3.o