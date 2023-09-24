# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image`

all: display grayscale rotation luminance

SRC = display.c grayscale.c rotation.c luminance.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

display: display.o
grayscale: grayscale.o
rotation: rotation.o
luminance: luminance.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
