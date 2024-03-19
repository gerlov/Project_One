# Makefile for Mac
# .c files
SRCDIR=./source
# .h files
INCDIR=./include

# Name of the executable
TARGET:=Game
# Compiler
CC=:gcc
ifeq ($(OS),Windows_NT)
	# Windows stuff
	INCLUDE = C:\msys64\mingw64\include\SDL2 # SDL2 include path
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_ttf -mwindows -lm
	TARGET:=$(TARGET).exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		
		# compiler for mac
		CC:=$(CC)-13 
		# Mac stuff		
		INCLUDE = `sdl2-config --prefix`/include/SDL2 # SDL2 include path
		LDFLAGS = `sdl2-config --cflags --libs ` -lSDL2_image -lSDL2_ttf
		
	endif
endif
CFLAGS = -g -I$(INCLUDE) -I$(INCDIR) -c


# Add compiling steps here

$(TARGET): main.o
	$(CC) -o $(TARGET) main.o $(LDFLAGS)


main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

test:
	@echo $(CC)
	@echo $(INCLUDE)
	@echo $(UNAME_S)
	@echo $(CFLAGS)
	@echo $(LDFLAGS)
	@echo $(SRCDIR)
	@echo $(TARGET)
run: $(TARGET)
	./$(TARGET)
clean:
	rm *.o $(TARGET)