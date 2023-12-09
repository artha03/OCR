#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include "resize.h"

void resize_image(const char *input_filename, int new_width, int new_height,int i) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error SDL : %s\n", SDL_GetError());
        return;
    }

    SDL_Surface *original_surface = IMG_Load(input_filename);
    if (!original_surface) {
        fprintf(stderr, "Error loading image : %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Surface *temp_surface = SDL_ConvertSurfaceFormat(original_surface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (!temp_surface) {
        fprintf(stderr, "Error converting surface format: %s\n", SDL_GetError());
        SDL_FreeSurface(original_surface);
        SDL_Quit();
        return;
    }

    SDL_Surface *resized_surface = SDL_CreateRGBSurface(0, new_width, new_height, 32,
                                                        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (!resized_surface) {
        fprintf(stderr, "Error creating resized surface: %s\n", SDL_GetError());
        SDL_FreeSurface(original_surface);
        SDL_FreeSurface(temp_surface);
        SDL_Quit();
        return;
    }

    SDL_BlitScaled(temp_surface, NULL, resized_surface, NULL);

    const char *last_dot = strrchr(input_filename, '.');
    if (last_dot != NULL) {
        last_dot = '\0';
    }

    char path[100];
    //printf("%s\n", path);
    sprintf(path, "Decoupage/resize/Image_%d.png", i);
    IMG_SavePNG(resized_surface, path);

    SDL_FreeSurface(original_surface);
    SDL_FreeSurface(temp_surface);
    SDL_FreeSurface(resized_surface);
    SDL_Quit();
    //printf("Image resized successfully\n");

}


void invert_colors(const char *input_filename, const char *output_filename) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error SDL : %s\n", SDL_GetError());
        return;
    }

    SDL_Surface *original_surface = IMG_Load(input_filename);
    if (!original_surface) {
        fprintf(stderr, "Error loading image : %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_LockSurface(original_surface);

    // Inversion des couleurs : noirs en blanc, blancs en noirs
    Uint32 *pixels = (Uint32 *)original_surface->pixels;
    int pixel_count = original_surface->w * original_surface->h;
    for (int i = 0; i < pixel_count; ++i) {
        Uint32 pixel = pixels[i];
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, original_surface->format, &r, &g, &b, &a);

        // Inversion : noir (0,0,0) en blanc (255,255,255) et vice versa
        r = (r == 0) ? 255 : 0;
        g = (g == 0) ? 255 : 0;
        b = (b == 0) ? 255 : 0;

        pixels[i] = SDL_MapRGBA(original_surface->format, r, g, b, a);
    }

    SDL_UnlockSurface(original_surface);

    // Sauvegarde de l'image avec les couleurs inversées
    if (IMG_SavePNG(original_surface, output_filename) != 0) {
        fprintf(stderr, "Error saving image : %s\n", IMG_GetError());
    }

    SDL_FreeSurface(original_surface);
    SDL_Quit();
}

int res()
{
    for (int i = 0; i < 81; i++) {

        char *input_filename = malloc(100);
        sprintf(input_filename, "Decoupage/Image_%d.jpeg", i);
        int new_width = 28;
        int new_height = 28;

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "Error SDL : %s\n", SDL_GetError());
            return 1;
        }

        if (IMG_Init(IMG_INIT_PNG) == 0) {
            fprintf(stderr, "Error SDL : %s\n", IMG_GetError());
            SDL_Quit();
            return 1;
        }

        resize_image(input_filename, new_width, new_height,i);

        IMG_Quit();
        SDL_Quit();
    }

    for (int i = 0; i < 81; i++) {

        char *input_filename = malloc(100);
        sprintf(input_filename, "Decoupage/resize/Image_%d.png", i);

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "Error SDL : %s\n", SDL_GetError());
            return 1;
        }

        if (IMG_Init(IMG_INIT_PNG) == 0) {
            fprintf(stderr, "Error SDL : %s\n", IMG_GetError());
            SDL_Quit();
            return 1;
        }

        invert_colors(input_filename, input_filename);

        IMG_Quit();
        SDL_Quit();
    }



    return 0;
}

/*
int main() {

    for (int i = 0; i < 81; i++) {

        char *input_filename = malloc(100);
        sprintf(input_filename, "Decoupage/Image_%d.jpeg", i);
        int new_width = 28;
        int new_height = 28;

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "Error SDL : %s\n", SDL_GetError());
            return 1;
        }

        if (IMG_Init(IMG_INIT_PNG) == 0) {
            fprintf(stderr, "Error SDL : %s\n", IMG_GetError());
            SDL_Quit();
            return 1;
        }

        resize_image(input_filename, new_width, new_height,i);

        IMG_Quit();
        SDL_Quit();
    }

    for (int i = 0; i < 81; i++) {

        char *input_filename = malloc(100);
        sprintf(input_filename, "Decoupage/resize/Image_%d.png", i);

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "Error SDL : %s\n", SDL_GetError());
            return 1;
        }

        if (IMG_Init(IMG_INIT_PNG) == 0) {
            fprintf(stderr, "Error SDL : %s\n", IMG_GetError());
            SDL_Quit();
            return 1;
        }

        invert_colors(input_filename, input_filename);

        IMG_Quit();
        SDL_Quit();
    }



    return 0;
}*/