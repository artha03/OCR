#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL2_rotozoom.h>

#include "binarisation.h"
#include "grayscale.h"
#include "reducBruit.h"
#include "Rot.h"

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 4)
        errx(EXIT_FAILURE, "Usage: image-file");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a window.
    SDL_Window* window = SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0, 0, 0,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a renderer.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(argv[1]);
    if (surface == NULL)
        errx(EXIT_FAILURE,"%s", SDL_GetError());


    SDL_SetWindowSize(window, surface->w,surface->h);

    SDL_Texture* texture = IMG_LoadTexture(renderer, argv[1]);
    if (texture == NULL)
        errx(EXIT_FAILURE,"%s", SDL_GetError());
    //draw(renderer,texture);

    SDL_Surface *resultatSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0, 0, 0, 0);

    //surface_to_grayscale(resultatSurface);
    //surface_to_flou(resultatSurface);

    //applySharpnessFilter(surface);
    //contrast(surface);

    //applySmoothingFilter(surface);
    reduction_Bruit(surface,resultatSurface);
    //contrast(resultatSurface);
    convertToGray(resultatSurface);

    surface_to_bin(resultatSurface);

    double angle_rotation = atof(argv[2]);
    SDL_Surface * resultatSurface2 = rotozoomSurface(resultatSurface, angle_rotation, 1.0, 0);


    SDL_Texture * texture_change = SDL_CreateTextureFromSurface(renderer, resultatSurface);

    SDL_SaveBMP(resultatSurface2, argv[3]);

    // Dispatches the events.
    //event_loop(renderer,texture,texture_change);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(resultatSurface);



    // Destroys the objects.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture_change);
    SDL_DestroyTexture(texture);
    //SDL_DestroySurface(surface);
    SDL_Quit();

    return EXIT_SUCCESS;
}