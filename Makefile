# Makefile for both Windows and Mac
# folder for .c files 

# DO NOT USE

# SRCDIR=./source
# # folder for .h files
# INCDIR=./include

# # Name of the executable
SERVER:=Server
CLIENT:=Client
# # Compiler
# CC:=gcc

# SDL_FLAGS=-lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer
ifeq ($(OS),Windows_NT)
	# # For Windows
	# INCLUDE=C:\msys64\mingw64\include\ # SDL2 include path
	# LDFLAGS=-lmingw32 -lSDL2main -lSDL2_image -lSDL2 $(SDL_FLAGS) -mwindows -lm
	SERVER:=$(SERVER).exe
	CLIENT:=$(CLIENT).exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		# # For Mac
		
		# # compiler for Mac
		# CC:=$(CC)-13
		# # make sure to install SDL2 using brew
		# # and that 'sdl2-config --prefix' returns '/opt/homebrew' in the terminal
		# INCLUDE=`sdl2-config --prefix`/include/ # SDL2 include path
		# LDFLAGS=`sdl2-config --cflags --libs ` $(SDL_FLAGS) 
		SERVER:=$(SERVER).out
		CLIENT:=$(CLIENT).out
	endif
endif
# CFLAGS=-g -I$(INCLUDE) -I$(INCDIR) -c

# ----- Compile the program -----------------
all: client server

client: 
	$(MAKE) -C Client/ $(CLIENT)

server:
	$(MAKE) -C Server/ $(SERVER)


clean:
	$(MAKE) -C Client/ clean
	$(MAKE) -C Server/ clean

runserver:
	$(MAKE) -C Server/ run

runclient:
	$(MAKE) -C Client/ run
	
.PHONY: All client server clean runServer runClient

# run 'make' to compile the program

# $(TARGET): main.o tilemap.o music.o window.o collisions.o powerup.o menu.o character.o texture.o game.o limitedvision.o
# 	$(CC) -o $@ $^ $(LDFLAGS)


# main.o: $(SRCDIR)/main.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/main.c
	
# tilemap.o: $(SRCDIR)/tilemap.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/tilemap.c

# music.o: $(SRCDIR)/music.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/music.c

# window.o: $(SRCDIR)/window.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/window.c  

# collisions.o: $(SRCDIR)/collisions.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/collisions.c	

# powerup.o: $(SRCDIR)/powerup.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/powerup.c		

# menu.o: $(SRCDIR)/menu.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/menu.c

# character.o: $(SRCDIR)/character.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/character.c

# texture.o: $(SRCDIR)/texture.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/texture.c

# game.o: $(SRCDIR)/game.c
# 	$(CC) $(CFLAGS) $(SRCDIR)/game.c

# limitedvision.o: $(SRCDIR)/limitedvision.c $(INCDIR)/limitedvision.h
# 	$(CC) $(CFLAGS) $(SRCDIR)/limitedvision.c

# # ----- Run the program -----------------
# # run 'make run' to compile and run the program in one command
# run: $(TARGET)
# 	./$(TARGET)

# # run 'make clean' to remove the object files and the executable
# clean:
# ifeq ($(OS),Windows_NT)
# 	del /Q /F *.o *.exe
# else
# 	rm -f *.o $(TARGET)
# endif