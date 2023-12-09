# Makefile for Sudoku Solver

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lSDL2_ttf -lm

all: main

SRC = solver.c res_sudoku.c main.c
OBJ = ${SRC:.c=.o}

main: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o main $(LDFLAGS) $(LDLIBS)

.PHONY: clean

clean:
	$(RM) $(OBJ) main

# END