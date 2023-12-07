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

Uint8 *pixelref(SDL_Surface *surface, int posx, int posy)
{
    int bpp = surface->format->BytesPerPixel;
    return (Uint8 *)surface->pixels + posy * surface->pitch + posx * bpp;
}

Uint32 SDL_GetPixel(SDL_Surface *surface, int posx, int posy)
{
    Uint8 *pixel = pixelref(surface, posx, posy);
    switch (surface->format->BytesPerPixel)
    {
    case 1:
        return *pixel;
    case 2:
        return *(Uint16 *)pixel;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
        else
            return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
    case 4:
        return *(Uint32 *)pixel;
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
            *(Uint32 *)pixel = color;
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

// Structure représentant un point dans l'espace 2D
// Structure représentant un point dans l'espace 2D
typedef struct
{
    double x;
    double y;
} Point;

// Fonction pour trouver l'intersection de deux lignes
Point findIntersection(Line line1, Line line2, int maxDist)
{
    double a1 = cos(line1.theta * M_PI / 180.0);
    double b1 = sin(line1.theta * M_PI / 180.0);
    double x01 = a1 * (line1.rho - maxDist / 2);
    double y01 = b1 * (line1.rho - maxDist / 2);

    double a2 = cos(line2.theta * M_PI / 180.0);
    double b2 = sin(line2.theta * M_PI / 180.0);
    double x02 = a2 * (line2.rho - maxDist / 2);
    double y02 = b2 * (line2.rho - maxDist / 2);

    double det = a1 * b2 - a2 * b1;

    if (fabs(det) < 1e-5)
    {
        // Les lignes sont parallèles, retourne un point indéfini
        return (Point){INFINITY, INFINITY};
    }
    else
    {
        double x = (b2 * x01 - b1 * x02) / det;
        double y = (a1 * y02 - a2 * y01) / det;
        return (Point){x, y};
    }
}

// Fonction pour trouver les coins de la grille à partir des lignes détectées
void findGridCorners(Line lines[], int linesCount, int maxDist)
{
    for (int i = 0; i < linesCount - 1; ++i)
    {
        for (int j = i + 1; j < linesCount; ++j)
        {
            Point intersection = findIntersection(lines[i], lines[j], maxDist);
            // Si l'intersection est à l'intérieur de l'image, affichez les coordonnées
            if (intersection.x >= 0 && intersection.x < WIDTH && intersection.y >= 0 && intersection.y < HEIGHT)
            {
                printf("Corner at (%f, %f)\n", intersection.x, intersection.y);
                // Vous pouvez stocker ces points dans un tableau ou les utiliser directement selon vos besoins.
            }
        }
    }
}

// Fonction pour calculer la distance euclidienne entre deux points
double distanceBetweenPoints(Point p1, Point p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Fonction pour calculer la distance spatiale entre deux lignes dans l'espace image
double distanceBetweenLines(Line line1, int rho2, int theta2, int maxDist)
{
    // Convertir les coordonnées polaires en coordonnées cartésiennes pour les deux lignes
    double a1 = cos(line1.theta * M_PI / 180.0);
    double b1 = sin(line1.theta * M_PI / 180.0);
    double x01 = a1 * (line1.rho - maxDist / 2);
    double y01 = b1 * (line1.rho - maxDist / 2);

    double a2 = cos(theta2 * M_PI / 180.0);
    double b2 = sin(theta2 * M_PI / 180.0);
    double x02 = a2 * (rho2 - maxDist / 2);
    double y02 = b2 * (rho2 - maxDist / 2);

    // Trouver les points les plus proches des deux lignes
    double t1 = a1 * x02 - b1 * y02 - x01;
    double t2 = b1 * x02 + a1 * y02 - y01;

    // Calculer et retourner la distance euclidienne entre les points les plus proches
    return sqrt(t1 * t1 + t2 * t2);
}

void houghTransform(SDL_Surface *cannyImage, SDL_Renderer *renderer)
{
    int maxDist = cannyImage->w + cannyImage->h;
    int thetaRes = 180;

    int **accumulator = malloc(maxDist * sizeof(int *));
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

                    if (fabs(cos(radians)) < 0.1 || fabs(sin(radians)) < 0.1)
                    {
                        double a = cos(radians);
                        double b = sin(radians);
                        double x0 = a * (rho - maxDist / 2);
                        double y0 = b * (rho - maxDist / 2);
                        double scale = 2000.0;

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

                        if (existingLineIndex == -1)
                        {
                            // Ajouter une nouvelle ligne seulement si elle n'est pas proche des autres lignes
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
    
    findGridCorners(lines, linesCount, maxDist);
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
