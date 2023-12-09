# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lSDL2_gfx -lm

all: transformeeHough filtreCanny  dec rogne test

SRC = transformeeHough3.c filtreCanny.c dec.c rogne.c test.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

transformeeHough : transformeeHough3.o
filtreCanny : filtreCanny.o
dec : dec.o
rogne: rogne.o
test : transformeeHough3.o filtreCanny.o dec.o rogne.o test.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
