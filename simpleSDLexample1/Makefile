# Makefile for Mac
# .c files
SRCDIR=./source
# .h files
INCDIR=./include
CC=gcc-13 # compiler


ifeq ($(OS),Windows_NT)
	# Windows stuff
	INCLUDE = C:\msys64\mingw64\include\SDL2 # SDL2 include path
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -mwindows -lm
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		# Mac stuff
		INCLUDE = `sdl2-config --prefix`/include/SDL2 # SDL2 include path
		LDFLAGS = `sdl2-config --cflags --libs ` -lSDL2_image -lSDL2_ttf
		
	endif
	ifeq ($(UNAME_S),Linux)
		# Linux stuff
	endif
endif
CFLAGS = -g -I$(INCLUDE) -I$(INCDIR) -c

Game: main.o
	$(CC) -o Game.exe main.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

test: 
	@echo $(UNAME_S)
	@echo $(CFLAGS)
	@echo $(LDFLAGS)
	@echo $(SRCDIR)
run: Game
	./Game.exe
clean:
	rm *.o Game.exe