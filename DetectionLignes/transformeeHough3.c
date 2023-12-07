#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#define WIDTH 800
#define HEIGHT 600
#define MAX_LINES 10000
typedef struct
{
    double rho;
    double theta;
} Line;

Uint8* pixelref(SDL_Surface *surface, int posx, int posy)
{
    int bpp = surface->format->BytesPerPixel;
    return (Uint8*)surface->pixels + posy * surface->pitch + posx * bpp;
}

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
}

void SDL_DrawLine(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        if (x1 >= 0 && x1 < surface->w && y1 >= 0 && y1 < surface->h)
        {
            Uint8 *pixel = pixelref(surface, x1, y1);
            *(Uint32*)pixel = color;
        }

        if (x1 == x2 && y1 == y2)
        {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}
///////////////////////////








//////////////////////////


void houghTransform(SDL_Surface *cannyImage, SDL_Renderer *renderer)
{
    int maxDist = cannyImage->w + cannyImage->h;
    int thetaRes = 180;

    int** accumulator = malloc(maxDist * sizeof(int*));
    for (int i = 0; i < maxDist; ++i)
    {
        accumulator[i] = malloc(thetaRes * sizeof(int));
    }
    for (int i = 0; i < maxDist; ++i)
    {
        for (int j = 0; j < thetaRes; ++j)
        {
            accumulator[i][j] = 0;
        }
    }

    double thresholdPercentage = 0.35;
    int maxAccumulatorValue = 0;

    for (int y = 0; y < cannyImage->h; ++y)
    {
        for (int x = 0; x < cannyImage->w; ++x)
        {
            if (SDL_GetPixel(cannyImage, x, y) > 128)
            {
                for (int theta = 0; theta < thetaRes; ++theta)
                {
                    double radians = theta * M_PI / 180.0;
                    int rho = (int)(x * cos(radians) + y * sin(radians));
                    rho += maxDist / 2;

                    rho = (rho + maxDist) % maxDist;
                    theta = theta % thetaRes;

                    if (rho >= 0 && rho < maxDist && theta >= 0 && theta < thetaRes)
                    {
                        accumulator[rho][theta]++;
                        if (accumulator[rho][theta] > maxAccumulatorValue)
                        {
                            maxAccumulatorValue = accumulator[rho][theta];
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Indices rho=%d, theta=%d hors limites.\n", rho, theta);
                    }
                }
            }
        }
    }

    double rhoThreshold = 20.0;
    double thetaThreshold = 5.0;

    Line lines[MAX_LINES];
    int linesCount = 0;

    for (int rho = 0; rho < maxDist; ++rho)
    {
        for (int theta = 0; theta < thetaRes; ++theta)
        {
            if (rho >= 0 && rho < maxDist && theta >= 0 && theta < thetaRes)
            {
                if (accumulator[rho][theta] > maxAccumulatorValue * thresholdPercentage)
                {
                    double radians = theta * M_PI / 180.0;

                    // Filtrer les lignes verticales ou horizontales
                    if (fabs(cos(radians)) < 0.1 || fabs(sin(radians)) < 0.1)
                    {
                        double a = cos(radians);
                        double b = sin(radians);
                        double x0 = a * (rho - maxDist / 2);
                        double y0 = b * (rho - maxDist / 2);
                        double scale = 2000.0;

                        // Vérifier si la ligne est proche d'une ligne déjà détectée
                        int existingLineIndex = -1;

                        for (int i = 0; i < linesCount; ++i)
                        {
                            Line currentLine = lines[i];
                            if (fabs(currentLine.rho - rho) < rhoThreshold &&
                                fabs(currentLine.theta - theta) < thetaThreshold)
                            {
                                existingLineIndex = i;
                                break;
                            }
                        }

                        if (existingLineIndex != -1)
                        {
                            // Mettre à jour la ligne existante avec la moyenne
                            Line existingLine = lines[existingLineIndex];
                            existingLine.rho = (existingLine.rho + rho) / 2.0;
                            existingLine.theta = (existingLine.theta + theta) / 2.0;
                        }
                        else
                        {
                            // Ajouter une nouvelle ligne
                            lines[linesCount].rho = rho;
                            lines[linesCount].theta = theta;
                            linesCount++;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < linesCount; ++i)
    {
        Line currentLine = lines[i];
        double radians = currentLine.theta * M_PI / 180.0;
        double a = cos(radians);
        double b = sin(radians);
        double x0 = a * (currentLine.rho - maxDist / 2);
        double y0 = b * (currentLine.rho - maxDist / 2);
        double scale = 2000.0;

        SDL_DrawLine(cannyImage, (int)(x0 - scale * b), (int)(y0 + scale * a),
                     (int)(x0 + scale * b), (int)(y0 - scale * a), 0xFF0000);
    }

    for (int i = 0; i < maxDist; ++i)
    {
        free(accumulator[i]);
    }
    free(accumulator);
}


int main(int argc, char ** argv) {

    if(argc != 3)
        return EXIT_FAILURE;


    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

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
    }

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) {
        fprintf(stderr, "Erreur lors du chargement de l'image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    houghTransform(image, renderer);


    if(SDL_SaveBMP(image, argv[2]) != 0) {
        fprintf(stderr, "Erreur lors de la sauvegarde de l'image: %s\n", SDL_GetError());
    }


    SDL_FreeSurface(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
