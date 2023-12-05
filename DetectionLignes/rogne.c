#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdlib.h>



#include <math.h>

Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        return *p;
    case 2:
        return *(Uint16 *)p;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32 *)p;
    default:
        return 0;
    }
}

SDL_Surface *load_image(const char *path) {
    SDL_Surface *image = IMG_Load(path);
    if (!image) {
        printf("Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        exit(1);
    }
    return image;
}

void hough_transform(SDL_Surface *image, float *accumulator, int *acc_width, int *acc_height) {
    int width = image->w;
    int height = image->h;
    *acc_width = (int)sqrt(width * width + height * height);
    *acc_height = 180;  // Angle range: -90 to 90 degrees

    // Initialiser l'accumulateur à zéro
    for (int i = 0; i < *acc_width * *acc_height; ++i) {
        accumulator[i] = 0;
    }

    // Parcourir l'image pour voter dans l'accumulateur
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = get_pixel(image, x, y);
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);

            // Vérifier si le pixel est rouge
            if (r > 200 && g < 100 && b < 100) {
                // Voter pour toutes les lignes possibles pour ce point
                for (int theta = 0; theta < 180; ++theta) {
                    float radian_theta = theta * M_PI / 180.0;
                    int rho = (int)(x * cos(radian_theta) + y * sin(radian_theta));
                    accumulator[theta * *acc_width + rho]++;
                }
            }
        }
    }
}

void find_square(int *accumulator, int acc_width, int acc_height, SDL_Rect *square_rect) {
    int threshold = 100;  // Ajuster en fonction de votre image
    int min_vote = 0;

    // Trouver le carré avec le plus de votes
    for (int theta = 0; theta < acc_height; ++theta) {
        for (int rho = 0; rho < acc_width; ++rho) {
            if (accumulator[theta * acc_width + rho] > min_vote) {
                min_vote = accumulator[theta * acc_width + rho];

                // Calculer les coordonnées du carré
                double radian_theta = theta * M_PI / 180.0;
                int x0 = (int)(rho * cos(radian_theta));
                int y0 = (int)(rho * sin(radian_theta));

                // Ajuster les coordonnées pour rester dans les limites de l'image
                x0 = (x0 < 0) ? 0 : (x0 >= acc_width) ? acc_width - 1 : x0;
                y0 = (y0 < 0) ? 0 : (y0 >= acc_height) ? acc_height - 1 : y0;

                square_rect->x = x0;
                square_rect->y = y0;
                square_rect->w = acc_width;
                square_rect->h = acc_height;
            }
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Surface *image = load_image("Hough2.jpeg");

    int acc_width, acc_height;
    float *accumulator;
    accumulator = (float *)malloc(acc_width * acc_height * sizeof(float));

    // Appliquer la transformation de Hough pour détecter les lignes
    hough_transform(image, accumulator, &acc_width, &acc_height);

    // Trouver le carré basé sur l'accumulateur de Hough
    SDL_Rect square_rect;
    find_square(accumulator, acc_width, acc_height, &square_rect);

    // Rogner l'image pour conserver uniquement le grand carré
    SDL_Surface *croppedSurface = SDL_CreateRGBSurface(0, square_rect.w, square_rect.h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(image, &square_rect, croppedSurface, NULL);

    // Enregistrer l'image rognée si nécessaire
    IMG_SavePNG(croppedSurface, "res2.jpeg");

    // Libérer les surfaces et l'accumulateur
    free(accumulator);
    SDL_FreeSurface(image);
    SDL_FreeSurface(croppedSurface);

    IMG_Quit();
    SDL_Quit();
    return 0;
}
