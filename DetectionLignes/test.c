#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "err.h"

#include "dec.h"
#include "filtreCanny.h"
#include "transformeeHough3.h"
#include "rogne.h"
#include "resize.h"

int main(int argc, char ** argv) {

    if(argc != 3)
        return EXIT_FAILURE;


    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /*
    SDL_Window *window = SDL_CreateWindow("Transformée de Hough", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    if (!window) {
        fprintf(stderr, "Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Erreur lors de la création du rendu: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }*/

    SDL_Surface *image = load_image(argv[1]);
    if (!image) {
        fprintf(stderr, "Erreur lors du chargement de l'image: %s\n", IMG_GetError());
        //SDL_DestroyRenderer(renderer);
        //SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    applySobel(image);

    houghTransform(image);

    detectSudokuGrid(image);

    int CELL_SIZE_X = image->w /9;
    int CELL_SIZE_Y = image->h /9;

    new_img(image,CELL_SIZE_X,CELL_SIZE_Y);

    int err = res();
    if(err!= 0) {
        fprintf(stderr, "Erreur lors de la sauvegarde de l'image: %s\n", SDL_GetError());
    }


    if(SDL_SaveBMP(image, argv[2]) != 0) {
        fprintf(stderr, "Erreur lors de la sauvegarde de l'image: %s\n", SDL_GetError());
    }


    SDL_FreeSurface(image);
    //SDL_DestroyRenderer(renderer);
    //SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}