# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lSDL2_gfx

#all: display grayscale binarisation rotation luminance flou filtres reducBruit nonMaxima Rot test
all: grayscale binarisation reducBruit Rot test

#SRC = display.c grayscale.c binarisation.c rotation.c luminance.c flou.c filtres.c reducBruit.c nonMaxima.c Rot.c test.c
SRC = grayscale.c binarisation.c reducBruit.c Rot.c test.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

#display: display.o
grayscale: grayscale.o
binarisation: binarisation.o
#rotation: rotation.o
#luminance: luminance.o
#flou: flou.o
reducBruit: reducBruit.o
#nonMaxima: nonMaxima.o
Rot: Rot.o
test: grayscale.o binarisation.o reducBruit.o Rot.o test.o
.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
