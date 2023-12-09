# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lSDL2_gfx -lm

all: transformeeHough transformeeHough2 transformeeHough3 filtreCanny HoughTransform dec rogne

SRC = transformeeHough.c transformeeHough2.c transformeeHough3.c filtreCanny.c HoughTransform.c dec.c rogne.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

transformeeHough : transformeeHough.o
transformeeHough2 : transformeeHough2.o
transformeeHough3 : transformeeHough3.o
filtreCanny : filtreCanny.o
HoughTransform : HoughTransform.o
dec : dec.o
rogne: rogne.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
