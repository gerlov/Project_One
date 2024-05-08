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
	EXT:=.exe
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
		EXT:=.out
		
		
	endif
endif

SERVER:=$(SERVER)$(EXT)
CLIENT:=$(CLIENT)$(EXT)
# CFLAGS=-g -I$(INCLUDE) -I$(INCDIR) -c

# ----- Compile the program -----------------
all: client server

client: 
	$(MAKE) -C Client/ $(CLIENT)

server:
	$(MAKE) -C Server/ $(SERVER)

## had to add this (1st line)  to remove old buld files 
## from main project directory (win os env): del /Q /F *.o *.exe  
## also, added clean rules for win os into client and server makefiles respectively 
clean:
	del /Q /F *.o *.exe          
	$(MAKE) -C Client/ clean
	$(MAKE) -C Server/ clean

runserver:
	$(MAKE) -C Server/ run

runclient:
	$(MAKE) -C Client/ run
	
.PHONY: All client server clean runServer runClient
