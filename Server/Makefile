# Makefile for both Windows and Mac
# This Makefile compiles the server program
# folder for .c files 
BUILD_DIR=./build

SRCDIR=./src
COMMONSRCDIR=../lib/src
# folder for .h files
INCDIR=../lib/inc

# Name of the executable
TARGET:=Server
# Compiler
CC:=gcc

SDL_FLAGS=-lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer
ifeq ($(OS),Windows_NT)
	# For Windows
	INCLUDE=C:\msys64\mingw64\include\ # SDL2 include path
	LDFLAGS=-lmingw32 -lSDL2main -lSDL2_image -lSDL2 $(SDL_FLAGS)
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

all: $(TARGET)

# run 'make' to compile the program
$(TARGET): $(BUILD_DIR)/main.o $(BUILD_DIR)/tilemap.o $(BUILD_DIR)/music.o $(BUILD_DIR)/window.o $(BUILD_DIR)/collisions.o $(BUILD_DIR)/powerup.o $(BUILD_DIR)/mazeview.o $(BUILD_DIR)/menu.o $(BUILD_DIR)/character.o $(BUILD_DIR)/texture.o $(BUILD_DIR)/limitedvision.o $(BUILD_DIR)/our_rand.o $(BUILD_DIR)/maze_gen.o $(BUILD_DIR)/escape_portal.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/main.o: $(SRCDIR)/main.c $(INCDIR)/data.h $(INCDIR)/tilemap.h $(INCDIR)/window.h $(INCDIR)/music.h $(INCDIR)/powerup.h $(INCDIR)/character.h $(INCDIR)/texture.h $(INCDIR)/limitedvision.h $(INCDIR)/our_rand.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(SRCDIR)/main.c -o $@
	
$(BUILD_DIR)/tilemap.o: $(COMMONSRCDIR)/tilemap.c $(INCDIR)/tilemap.h $(INCDIR)/our_rand.h $(INCDIR)/maze_gen.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/tilemap.c -o $@  

$(BUILD_DIR)/music.o: $(COMMONSRCDIR)/music.c $(INCDIR)/music.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/music.c -o $@

$(BUILD_DIR)/window.o: $(COMMONSRCDIR)/window.c $(INCDIR)/window.h $(INCDIR)/music.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/window.c  -o $@

$(BUILD_DIR)/collisions.o: $(COMMONSRCDIR)/collisions.c $(INCDIR)/collisions.h $(INCDIR)/tilemap.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/collisions.c -o $@

$(BUILD_DIR)/powerup.o: $(COMMONSRCDIR)/powerup.c $(INCDIR)/powerup.h $(INCDIR)/tilemap.h $(INCDIR)/character.h $(INCDIR)/texture.h $(INCDIR)/music.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/powerup.c -o $@

$(BUILD_DIR)/mazeview.o: $(COMMONSRCDIR)/mazeview.c $(INCDIR)/mazeview.h $(INCDIR)/tilemap.h $(INCDIR)/texture.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) -c $(COMMONSRCDIR)/mazeview.c -o $@	

$(BUILD_DIR)/menu.o: $(COMMONSRCDIR)/menu.c $(INCDIR)/menu.h $(INCDIR)/music.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/menu.c -o $@

$(BUILD_DIR)/character.o: $(COMMONSRCDIR)/character.c $(INCDIR)/character.h $(INCDIR)/texture.h $(INCDIR)/collisions.h $(INCDIR)/tilemap.h $(INCDIR)/powerup.h $(INCDIR)/music.h $(INCDIR)/debug.h $(INCDIR)/escape_portal.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/character.c -o $@

$(BUILD_DIR)/texture.o: $(COMMONSRCDIR)/texture.c $(INCDIR)/texture.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/texture.c -o $@

$(BUILD_DIR)/limitedvision.o: $(COMMONSRCDIR)/limitedvision.c $(INCDIR)/limitedvision.h $(INCDIR)/tilemap.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/limitedvision.c -o $@

$(BUILD_DIR)/our_rand.o: $(COMMONSRCDIR)/our_rand.c $(INCDIR)/our_rand.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/our_rand.c -o $@

$(BUILD_DIR)/maze_gen.o: $(COMMONSRCDIR)/maze_gen.c $(INCDIR)/maze_gen.h $(INCDIR)/our_rand.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/maze_gen.c -o $@

$(BUILD_DIR)/escape_portal.o: $(COMMONSRCDIR)/escape_portal.c $(INCDIR)/escape_portal.h $(INCDIR)/tilemap.h $(INCDIR)/texture.h $(INCDIR)/debug.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/escape_portal.c -o $@

# ----- Run the program -----------------
# run 'make run' to compile and run the program in one command
run: $(TARGET)
	./$(TARGET)

# run 'make clean' to remove the object files and the executable
clean:
ifeq ($(OS),Windows_NT)
	del /Q /F "$(BUILD_DIR)\*.o" "$(BUILD_DIR)\*.exe"
else
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)
endif

.PHONY: all clean run