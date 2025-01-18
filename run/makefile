CC = gcc
CFLAGS = -lncurses -lmenu -I./*.h
SOURCES = ./main.c
all: main run clean

main:
	$(CC) $(SOURCES) $(CFLAGS) -o main

run:
	./main

clean:
	rm main
