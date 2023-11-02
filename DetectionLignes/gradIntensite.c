#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>

// Fonction pour calculer les gradients avec l'opérateur de Sobel
void calculerGradients(SDL_Surface *image, float *gradientsX, float *gradientsY) {
    int w = image->w;
    int h = image->h;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            Uint8 *pixelUpLeft = (Uint8 *)image->pixels + (y - 1) * image->pitch + (x - 1);
            Uint8 *pixelUp = pixelUpLeft + image->pitch;
            Uint8 *pixelUpRight = pixelUp + image->pitch;
            Uint8 *pixelLeft = pixelUpLeft + 1;
            Uint8 *pixelRight = pixelUpRight + 1;
            Uint8 *pixelDownLeft = pixelLeft + image->pitch;
            Uint8 *pixelDown = pixelDownLeft + image->pitch;
            Uint8 *pixelDownRight = pixelDown + 1;

            float gradientX = (*pixelUpRight + 2 * *pixelRight + *pixelDownRight) -
                              (*pixelUpLeft + 2 * *pixelLeft + *pixelDownLeft);

            float gradientY = (*pixelDownLeft + 2 * *pixelDown + *pixelDownRight) -
                              (*pixelUpLeft + 2 * *pixelUp + *pixelUpRight);

            gradientsX[y * w + x] = gradientX;
            gradientsY[y * w + x] = gradientY;
        }
    }
}

int main() {
    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    // Charger l'image avec SDL_image
    SDL_Surface *imageSurface = IMG_Load("chemin/vers/ton/image.jpg");
    if (!imageSurface) {
        printf("SDL_image could not load the image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    int w = imageSurface->w;
    int h = imageSurface->h;

    // Créer une fenêtre SDL
    SDL_Window *window = SDL_CreateWindow("Calcul des gradients", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);

    // Créer des tableaux pour les gradients X et Y
    float *gradientsX = malloc(w * h * sizeof(float));
    float *gradientsY = malloc(w * h * sizeof(float));

    // Calculer les gradients
    calculerGradients(imageSurface, gradientsX, gradientsY);

    // Libérer les ressources SDL
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(imageSurface);

    // Quitter SDL
    SDL_Quit();

    // Libérer la mémoire des tableaux de gradients
    free(gradientsX);
    free(gradientsY);

    return 0;
}
