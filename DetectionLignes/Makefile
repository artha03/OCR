# Makefile

CC = gcc
CPPFLAGS =
CFLAGS =
LDFLAGS =
LDLIBS = -lSDL2 -lSDL2_image -lm

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
