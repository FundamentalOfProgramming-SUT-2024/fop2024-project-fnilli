CC = gcc
CFLAGS = -lncurses -lmenu -lSDL2 -lSDL2_mixer -I./*.h
SOURCES = ./main.c
all: main run clean

main:
	$(CC) $(SOURCES) $(CFLAGS) -o main

run:
	./main

clean:
	rm main
