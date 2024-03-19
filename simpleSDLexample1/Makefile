# Makefile for Windows
SRCDIR=./source
CC=gcc

INCLUDE = /usr/local/include/SDL2

CFLAGS = -g -I$(INCLUDE) -c 
LDFLAGS = -L/usr/local/lib -lSDL2 -lSDL2_image -lm

simpleSDLexample1: main.o
	@echo "Linking..."
	$(CC) main.o -o simpleSDLexample1 $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	@echo "Compiling..."
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

clean:
	rm *.exe
	rm *.o