CC = gcc
CFLAGS = -lncurses -lmenu -I./*.h
SOURCES = ./*.c
all: rogue run clean

rogue:
	$(CC) $(SOURCES) $(CFLAGS) -o rogue

run:
	./rogue

clean:
	rm rogue
