#include "transformeeHough.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

#define MAX_LINES 100
#define MAX_RHO 800
#define MAX_THETA 180


typedef struct
{
    double rho;
    double theta;
} HoughLine;
void rho_theta_to_line(double rho, double theta, int* x1, int* y1, int* x2, int* y2)
{
    *x1 = (int)(rho *cos(theta));
    *y1 = (int)(rho *sin(theta));
    int lineLength = 100;
    *x2 =*x1 + (int)(lineLength * cos(theta));
    *y2 =*y1 + (int)(lineLength * sin(theta));
}


void drawHoughLine(SDL_Surface *surface, HoughLine line, Uint32 color)
{
    double cosTheta = cos(line.theta);
    double sinTheta = sin(line.theta);

    for (int i = 0; i < WIDTH; i++)
    {
        int posx = i;
        int posy = (int)((line.rho - posx * cosTheta) / sinTheta);

        if (posx >= 0 && posx < WIDTH && posy >= 0 && posy < HEIGHT)
        {
            ((Uint32 *)surface->pixels)[posy * WIDTH + posx] = color;
        }
    }

    for (int i = 0; i < HEIGHT; i++)
    {
        int posy = i;
        int posx = (int)((line.rho - posy * sinTheta) / cosTheta);

        if (posx >= 0 && posx < WIDTH && posy >= 0 && posy < HEIGHT)
        {
            ((Uint32 *)surface->pixels)[posy * WIDTH + posx] = color;
        }
    }
}

void houghTransform(SDL_Surface *image, SDL_Surface *output, Uint32 seuil)
{
    Uint32 *pixels = (Uint32 *)image->pixels;
    Uint32 *outputPixels = (Uint32 *)output->pixels;

    int maxDistance = (int)sqrt(WIDTH * WIDTH + HEIGHT * HEIGHT);
    int n_angle = 180;
    int *accumulator = calloc(maxDistance * n_angle, sizeof(int));

    for (int posy = 0; posy < HEIGHT; posy++)
    {
        for (int posx = 0; posx < WIDTH; posx++)
        {
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;
            SDL_GetRGBA(pixels[posy * WIDTH + posx], image->format, &r, &g, &b, &a);

            if (r > 200 && g > 200 && b > 200)
            {
                for (int angle = 0; angle < n_angle; angle++)
                {
                    double theta = angle * M_PI / n_angle;
                    int distance = (int)(posx * cos(theta) + posy * sin(theta));
                    distance += maxDistance / 2; // Offset pour des distances négatives
                    accumulator[angle * maxDistance + distance]++;
                }
            }
        }
    }

    for (int angle = 0; angle < n_angle; angle++)
    {
        for (int distance = 0; distance < maxDistance; distance++)
        {
            if (accumulator[angle * maxDistance + distance] > seuil)
            {
                HoughLine line;
                line.rho = distance - maxDistance / 2;
                line.theta = angle * M_PI / n_angle;
                drawHoughLine(output, line, SDL_MapRGB(output->format, 255, 0, 0));
            }
        }
    }

    free(accumulator);
}

int main(int argc, char ** argv)
{

    if(argc != 3)
        return EXIT_FAILURE;

    SDL_Init(SDL_INIT_VIDEO);

    // Initialisation de SDL_image
    if (IMG_Init(IMG_INIT_JPG) != IMG_INIT_JPG)
    {
        printf("Erreur lors de l'initialisation de SDL_image : %s\n", IMG_GetError());
        return -1;
    }

    SDL_Surface *inputImage = IMG_Load(argv[1]);
    SDL_Surface *outputImage = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);

    houghTransform(inputImage, outputImage, 1000);


    int hough[MAX_RHO][MAX_THETA];

    for (int rho = 0;rho<MAX_RHO;rho++)
    {
        for (int theta=0;theta<MAX_THETA;theta++)
            hough[rho][theta]=0;
    }
    int seuil= 100;

    SDL_Surface *image_copy = SDL_ConvertSurfaceFormat(inputImage, SDL_PIXELFORMAT_RGB888, 0);
    SDL_Window *windowres = SDL_CreateWindow("Hough Transform", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *rendererres = SDL_CreateRenderer(windowres, -1, SDL_RENDERER_ACCELERATED);
    for (int rho = 0; rho < MAX_RHO; rho++)
    {
        for (int theta = 0; theta < MAX_THETA; theta++)
        {
            if (hough[rho][theta] > seuil)
            {
                int x1, y1, x2, y2;
                rho_theta_to_line(rho, theta, &x1, &y1, &x2, &y2);

                SDL_RenderDrawLine(rendererres, x1, y1, x2, y2);
            }
        }
    }




    SDL_Texture *texture = SDL_CreateTextureFromSurface(rendererres, image_copy);
    SDL_RenderCopy(rendererres, texture, NULL, NULL);
    SDL_RenderPresent(rendererres);

    SDL_SaveBMP(outputImage, argv[2]);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rendererres);
    SDL_DestroyWindow(windowres);
    SDL_FreeSurface(inputImage);
    SDL_FreeSurface(outputImage);
    SDL_FreeSurface(image_copy);

    IMG_Quit();

    SDL_Quit();

    return EXIT_SUCCESS;
}
