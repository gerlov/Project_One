# Makefile for both Windows and Mac
# This Makefile compiles the client program
# folder for .c files 
SRCDIR=src
COMMONSRCDIR=../lib/source
# folder for .h files
INCDIR=../lib/include

# Name of the executable
TARGET:=Client
# Compiler
CC:=gcc

SDL_FLAGS=-lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer
ifeq ($(OS),Windows_NT)
	# For Windows
	INCLUDE=C:\msys64\mingw64\include\ # SDL2 include path
	LDFLAGS=-lmingw32 -lSDL2main -lSDL2_image -lSDL2 $(SDL_FLAGS) -mwindows -lm
	TARGET:=$(TARGET).exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		# For Mac
		
		# compiler for Mac
		CC:=$(CC)-13
		# make sure to install SDL2 using brew
		# and that 'sdl2-config --prefix' returns '/opt/homebrew' in the terminal
		INCLUDE=`sdl2-config --prefix`/include/ # SDL2 include path
		LDFLAGS=`sdl2-config --cflags --libs ` $(SDL_FLAGS) 
		TARGET:=$(TARGET).out
		
	endif
endif
CFLAGS=-g -I$(INCLUDE) -I$(INCDIR) -c

# ----- Compile the program -----------------

# run 'make' to compile the program
$(TARGET): main.o tilemap.o music.o window.o collisions.o powerup.o menu.o character.o texture.o game.o limitedvision.o our_rand.o
	$(CC) -o $@ $^ $(LDFLAGS)


main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c
	
tilemap.o: $(COMMONSRCDIR)/tilemap.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/tilemap.c

music.o: $(COMMONSRCDIR)/music.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/music.c

window.o: $(COMMONSRCDIR)/window.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/window.c  

collisions.o: $(COMMONSRCDIR)/collisions.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/collisions.c	

powerup.o: $(COMMONSRCDIR)/powerup.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/powerup.c		

menu.o: $(COMMONSRCDIR)/menu.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/menu.c

character.o: $(COMMONSRCDIR)/character.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/character.c

texture.o: $(COMMONSRCDIR)/texture.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/texture.c

game.o: $(COMMONSRCDIR)/game.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/game.c

limitedvision.o: $(COMMONSRCDIR)/limitedvision.c $(INCDIR)/limitedvision.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/limitedvision.c
our_rand.o: $(COMMONSRCDIR)/our_rand.c
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/our_rand.c

# ----- Run the program -----------------
# run 'make run' to compile and run the program in one command
run: $(TARGET)
	./$(TARGET)

# run 'make clean' to remove the object files and the executable
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F *.o *.exe
else
	rm -f *.o $(TARGET)
endif