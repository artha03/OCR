#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "transformeeHough3.h"
#include "rogne.h"

#define WIDTH 800
#define HEIGHT 600

/*
Uint8* pixelref(SDL_Surface *surface, int posx, int posy)
{
    int bpp = surface->format->BytesPerPixel;
    return (Uint8*)surface->pixels + posy * surface->pitch + posx * bpp;
}*/

/*
Uint32 SDL_GetPixel(SDL_Surface *surface, int posx, int posy)
{
    Uint8 *pixel = pixelref(surface, posx, posy);
    switch (surface->format->BytesPerPixel)
    {
        case 1:
            return *pixel;
        case 2:
            return *(Uint16*)pixel;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
            else
                return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
        case 4:
            return *(Uint32*)pixel;
        default:
            return 0;
    }
}*/

void detectSudokuGrid(SDL_Surface *image) {
    SDL_Surface *binaryImage = SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);

    // Convert the image to binary (black and white) using a simple threshold
    for (int y = 0; y < image->h; ++y) {
        for (int x = 0; x < image->w; ++x) {
            Uint8 *pixel = pixelref(image, x, y);
            Uint8 gray = 0.3 * pixel[0] + 0.59 * pixel[1] + 0.11 * pixel[2];

            // Use a threshold of 128 (adjust as needed)
            Uint32 binaryColor = (gray < 128) ? 0x000000 : 0xFFFFFF;

            Uint8 *binaryPixel = pixelref(binaryImage, x, y);
            *(Uint32*)binaryPixel = binaryColor;
        }
    }

    // Find contours in the binary image (simple edge detection)
    for (int y = 1; y < binaryImage->h - 1; ++y) {
        for (int x = 1; x < binaryImage->w - 1; ++x) {
            Uint8 *pixel = pixelref(binaryImage, x, y);
            Uint8 *pixelAbove = pixelref(binaryImage, x, y - 1);
            Uint8 *pixelBelow = pixelref(binaryImage, x, y + 1);
            Uint8 *pixelLeft = pixelref(binaryImage, x - 1, y);
            Uint8 *pixelRight = pixelref(binaryImage, x + 1, y);

            if (*(Uint32*)pixel == 0xFFFFFF &&
                (*(Uint32*)pixelAbove == 0x000000 || *(Uint32*)pixelBelow == 0x000000 ||
                 *(Uint32*)pixelLeft == 0x000000 || *(Uint32*)pixelRight == 0x000000)) {
                // Edge detection: set the pixel to red for visualization
                *(Uint32*)pixel = 0xFF0000;
            }
        }
    }

    // You can use SDL_SaveBMP here to save the binary image for debugging

    SDL_FreeSurface(binaryImage);
}

/*
int main(int argc, char **argv) {
    if (argc != 3)
        return EXIT_FAILURE;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("Détection de la grille de Sudoku", SDL_WINDOWPOS_UNDEFINED,
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
    }

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) {
        fprintf(stderr, "Erreur lors du chargement de l'image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    detectSudokuGrid(image);

    if (SDL_SaveBMP(image, argv[2]) != 0) {
        fprintf(stderr, "Erreur lors de la sauvegarde de l'image: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}*/
