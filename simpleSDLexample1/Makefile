# Makefile for both Windows and Mac
# folder for .c files 
SRCDIR=./source
# folder for .h files
INCDIR=./include

# Name of the executable
TARGET:=Game
# Compiler
CC:=gcc
ifeq ($(OS),Windows_NT)
	# For Windows
	INCLUDE=C:\msys64\mingw64\include\SDL2 # SDL2 include path
	LDFLAGS=-lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -mwindows -lm
	TARGET:=$(TARGET).exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		# For Mac
		
		# compiler for Mac
		CC:=$(CC)-13
		# make sure to install SDL2 using brew
		# and that 'sdl2-config --prefix' returns '/opt/homebrew' in the terminal
		INCLUDE=`sdl2-config --prefix`/include/SDL2 # SDL2 include path
		LDFLAGS=`sdl2-config --cflags --libs ` -lSDL2_image -lSDL2_ttf
		TARGET:=$(TARGET).out
		
	endif
endif
CFLAGS=-g -I$(INCLUDE) -I$(INCDIR) -c

# ----- Compile the program -----------------

# run 'make' to compile the program
$(TARGET): main.o
	$(CC) -o $(TARGET) main.o $(LDFLAGS)


main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

# run 'make run' to compile and run the program in one command
run: $(TARGET)
	./$(TARGET)

# run 'make clean' to remove the object files and the executable
clean:
	rm *.o $(TARGET)