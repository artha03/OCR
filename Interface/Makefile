# Makefile

CC = gcc
CPPFLAGS =

CFLAGS = `pkg-config --cflags sdl2 SDL2_image` `pkg-config --cflags gtk+-3.0` #-Wall -Wextra -Werror -g #-fsanitize=address

LDFLAGS = -lm
LDLIBS = `pkg-config --libs sdl2 SDL2_image` `pkg-config --libs gtk+-3.0`
EXEC = OCR

all: main

SRC = glade2.c ../DetectionLignes/transformeeHough3.c ../DetectionLignes/filtreCanny.c ../DetectionLignes/dec.c ../DetectionLignes/rogne.c ../DetectionLignes/resize.c ../Solver/solver.c ../Solver/res_sudoku.c ../Solver/main.c ../TraitementImage/grayscale.c ../TraitementImage/binarisation.c ../TraitementImage/reducBruit.c ../TraitementImage/Rot.c #../NeuralNetwork/neural_network.c ../NeuralNetwork/apprentissage.c #NeuralNetwork/GetImages.c NeuralNetwork/GuessFunction.c NeuralNetwork/main.c NeuralNetwork/TrainAndShow.c NeuralNetwork/Write.c NeuralNetwork/Maths.c NeuralNetwork/NeuralNetwork.c Image/Detection/lign_detection.c Image/Detection/sobel.c Image/pre_processing/filters.c Image/pre_processing/transformation.c Image/Display/display.c Image/Detection/square_detection.c Image/Perspective/correct_perspective.c Image/Perspective/matrix.c Image/Segmentation/split.c Image/Rotation_Resize/resize.c Image/Rotation_Resize/interpolation.c Image/Rotation_Resize/rotation.c Solver/sudoLoader.c Solver/sudoSolver.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

main : ${OBJ}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main OCR
	${RM} test_* sudo_resolved
# END