#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "err.h"

#include "dec.h"

#define GRID_SIZE 9
#define THRESHOLD 100

/*
SDL_Surface* load_image(const char* path)
{
    SDL_Surface * image = IMG_Load(path);
    if (image == NULL)
        errx(EXIT_FAILURE,"%s", SDL_GetError());
    SDL_Surface * image_ = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(image);
    return image_;
}*/

/*
SDL_Surface * create_image(SDL_Surface* surface,int startX, int startY)
{
    SDL_Surface * new_surface = SDL_CreateRGBSurface(0, NEW_TAILLE, NEW_TAILLE, 32, 0, 0, 0, 0);

    Uint32 *pixels = surface->pixels;
    Uint32 *new_pixels = new_surface->pixels;

    int pitch = surface->pitch;
    int new_pitch = new_surface->pitch;
    int x = 0;

    for (int i = 0; i < NEW_TAILLE-1; ++i)
    {
        for (int j = 0; j < NEW_TAILLE-1; ++j)
        {
            new_pixels[i * new_pitch + j] = pixels[(i+startX) * pitch + (j+startY)];
            //printf("%d\n",x);
            x++;
        }
    }

    return new_surface;
}*/

void new_img(SDL_Surface * surface,int CELL_SIZE_X, int CELL_SIZE_Y)
{
    int x = 0;
    char s[50];
    char * baseName = "Decoupage/Image_";
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            SDL_Rect cellRect = {j * CELL_SIZE_X, i * CELL_SIZE_Y, CELL_SIZE_X, CELL_SIZE_Y};

            SDL_Surface* cellSurface = SDL_CreateRGBSurface(0, CELL_SIZE_X, CELL_SIZE_Y, 32, 0, 0, 0, 0);
            SDL_BlitSurface(surface, &cellRect, cellSurface, NULL);

            sprintf(s, "%s%d.jpeg", baseName, x);
            SDL_SaveBMP(cellSurface,s);
            x++;
            SDL_FreeSurface(cellSurface);
        }
    }
}

/*
SDL_Surface * decoupage_rouge(SDL_Surface * formattedImage)
{
    // Parcourir chaque pixel et découper l'image en fonction de la détection de la couleur
    for (int y = 0; y < formattedImage->h; ++y) {
        for (int x = 0; x < formattedImage->w; ++x) {
            Uint32 pixel = *((Uint32*)formattedImage->pixels + y * formattedImage->w + x);

            Uint8 red, green, blue;
            SDL_GetRGB(pixel, formattedImage->format, &red, &green, &blue);

            if (red > THRESHOLD && green < THRESHOLD && blue < THRESHOLD)
            {
                // Tu as détecté un pixel rouge, fais le traitement nécessaire ici
                // Par exemple, tu pourrais stocker les coordonnées des lignes de la grille
            }
        }
    }
}*/


/*
int main(int argc, char ** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
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

    int CELL_SIZE_X = surface->w /9;
    int CELL_SIZE_Y = surface->h /9;

    SDL_SetWindowSize(window, surface->w,surface->h);

    SDL_Texture* texture = IMG_LoadTexture(renderer, argv[1]);
    if (texture == NULL)
        errx(EXIT_FAILURE,"%s", SDL_GetError());
    //draw(renderer,texture);

    //SDL_Surface *resultatSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0, 0, 0, 0);

    new_img(surface,CELL_SIZE_X,CELL_SIZE_Y);


    //SDL_SaveBMP(resultatSurface, argv[2]);

    SDL_FreeSurface(surface);
    //SDL_FreeSurface(resultatSurface);
    IMG_Quit();
    SDL_Quit();

    return 0;
}*/