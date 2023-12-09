#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "err.h"

#include "filtreCanny.h"

#define SOBEL_THRESHOLD 50
#define HIGH_THRESHOLD 50
#define LOW_THRESHOLD 20

void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}

SDL_Surface* load_image(const char* path)
{
    SDL_Surface * image = IMG_Load(path);
    if (image == NULL)
        errx(EXIT_FAILURE,"%s", SDL_GetError());
    SDL_Surface * image_ = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(image);
    return image_;
}

double convolution(SDL_Surface *image, int x, int y, int kernel[3][3]) {
    double result = 0.0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            Uint8 *pixel = (Uint8 *)image->pixels + (y + i) * image->pitch + (x + j) * 4;
            double value = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
            result += value * kernel[i + 1][j + 1];
        }
    }
    return result;
}
/*
void applySobel(SDL_Surface *image) {
    int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    SDL_Surface *temp = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);

    for (int y = 1; y < image->h - 1; y++) {
        for (int x = 1; x < image->w - 1; x++) {
            double Gx = convolution(temp, x, y, sobelX);
            double Gy = convolution(temp, x, y, sobelY);
            double gradient = sqrt(Gx * Gx + Gy * Gy);

            if (gradient > SOBEL_THRESHOLD) {
                Uint8 *pixel = (Uint8 *)image->pixels + y * image->pitch + x * 4;
                pixel[0] = pixel[1] = pixel[2] = 255; // Blanc
            } else {
                Uint8 *pixel = (Uint8 *)image->pixels + y * image->pitch + x * 4;
                pixel[0] = pixel[1] = pixel[2] = 0; // Noir
            }
        }
    }

    SDL_FreeSurface(temp);
}*/
void applySobel(SDL_Surface *image) {
    int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    SDL_Surface *temp = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);

    for (int y = 1; y < image->h - 1; y++) {
        for (int x = 1; x < image->w - 1; x++) {
            double Gx = convolution(temp, x, y, sobelX);
            double Gy = convolution(temp, x, y, sobelY);
            double gradient = sqrt(Gx * Gx + Gy * Gy);

            if (gradient > SOBEL_THRESHOLD) {
                Uint8 *pixel = (Uint8 *)image->pixels + y * image->pitch + x * 4;
                pixel[0] = pixel[1] = pixel[2] = 255; // Blanc
            } else {
                Uint8 *pixel = (Uint8 *)image->pixels + y * image->pitch + x * 4;
                pixel[0] = pixel[1] = pixel[2] = 0; // Noir
            }
        }
    }

    SDL_FreeSurface(temp);
}

void compareNeighbors(SDL_Surface *gradient, SDL_Surface *image, int x, int y, int dx, int dy) {
    Uint8 *center = (Uint8 *)gradient->pixels + y * gradient->pitch + x * 4;
    Uint8 *neighbor1 = (Uint8 *)gradient->pixels + (y + dy) * gradient->pitch + (x + dx) * 4;
    Uint8 *neighbor2 = (Uint8 *)gradient->pixels + (y - dy) * gradient->pitch + (x - dx) * 4;

    Uint8 *centerPixel = (Uint8 *)image->pixels + y * image->pitch + x * 4;
    //Uint8 *neighbor1Pixel = (Uint8 *)image->pixels + (y + dy) * image->pitch + (x + dx) * 4;
    //Uint8 *neighbor2Pixel = (Uint8 *)image->pixels + (y - dy) * image->pitch + (x - dx) * 4;

    if (center[0] < neighbor1[0] || center[0] < neighbor2[0]) {
        centerPixel[0] = centerPixel[1] = centerPixel[2] = 0;
    } else {
        centerPixel[0] = centerPixel[1] = centerPixel[2] = 255;
    }
}


void applyNonMaxSuppression(SDL_Surface *gradient, SDL_Surface *image) {
    for (int y = 1; y < gradient->h - 1; y++) {
        for (int x = 1; x < gradient->w - 1; x++) {
            Uint8 *pixel = (Uint8 *)gradient->pixels + y * gradient->pitch + x * 4;
            double angle = atan2(pixel[2], pixel[1]) * (180.0 / M_PI);

            if ((angle < 22.5 && angle >= -22.5) || (angle >= 157.5) || (angle < -157.5)) {
                compareNeighbors(gradient, image, x, y, 1, 0); // Horizontal
            } else if ((angle >= 22.5 && angle < 67.5) || (angle < -112.5 && angle >= -157.5)) {
                compareNeighbors(gradient, image, x, y, 1, -1); // Diagonal 1
            } else if ((angle >= 67.5 && angle < 112.5) || (angle < -67.5 && angle >= -112.5)) {
                compareNeighbors(gradient, image, x, y, 0, -1); // Vertical
            } else if ((angle >= 112.5 && angle < 157.5) || (angle < -22.5 && angle >= -67.5)) {
                compareNeighbors(gradient, image, x, y, -1, -1); // Diagonal 2
            }
        }
    }
}


void applyHysteresis(SDL_Surface *image) {
    for (int y = 1; y < image->h - 1; y++) {
        for (int x = 1; x < image->w - 1; x++) {
            Uint8 *pixel = (Uint8 *)image->pixels + y * image->pitch + x * 4;
            if (pixel[0] == 128) {
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        Uint8 *neighbor = (Uint8 *)image->pixels + (y + i) * image->pitch + (x + j) * 4;
                        if (neighbor[0] == 255) {
                            pixel[0] = pixel[1] = pixel[2] = 255;
                        }
                    }
                }
            }
        }
    }
}

void reduction_Bruit(SDL_Surface *image, SDL_Surface *resultat) {
    int fenetre = 3;

    for (int i = 0; i < image->h; i++)
    {
        for (int j = 0; j < image->w; j++)
        {
            int sommeR = 0;
            int sommeG = 0;
            int sommeB = 0;
            int pixelCount = 0;

            for (int k = -fenetre / 2; k <= fenetre / 2; k++)
            {
                for (int l = -fenetre / 2; l <= fenetre / 2; l++)
                {
                    int posx = j + l;
                    int posy = i + k;

                    if (posx >= 0 && posx < image->w && posy >= 0 && posy < image->h)
                    {
                        Uint32 pixel = ((Uint32 *)image->pixels)[posy * image->w + posx];
                        sommeR += pixel %256;
                        sommeG += (pixel >> 8) %256;
                        sommeB += (pixel >> 16) %256;
                        pixelCount++;
                    }
                }
            }
            Uint8 avgR = sommeR / pixelCount;
            Uint8 avgG = sommeG / pixelCount;
            Uint8 avgB = sommeB / pixelCount;
            ((Uint32 *)resultat->pixels)[i * resultat->w + j] = avgR | (avgG << 8) | (avgB << 16) | 0xFF000000;
        }
    }
}

void convertToGray(SDL_Surface *image) {
    for (int y = 0; y < image->h; ++y) {
        for (int x = 0; x < image->w; ++x) {
            Uint32 pixel = *((Uint32*)image->pixels + y * image->w + x);
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);
            Uint8 gray = 0.3 * r + 0.59 * g + 0.11 * b;
            *((Uint32*)image->pixels + y * image->w + x) = SDL_MapRGBA(image->format, gray, gray, gray, a);
        }
    }
}

/*
int main(int argc, char *argv[]) {

    // Checks the number of arguments.
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: image-file");

    // TODO:
    // - Initialize the SDL.
    // - Create a window.
    // - Create a renderer.
    // - Create a surface from the colored image.
    // - Resize the window according to the size of the image.
    // - Create a texture from the colored surface.
    // - Convert the surface into grayscale.
    // - Create a new texture from the grayscale surface.
    // - Free the surface.
    // - Dispatch the events.
    // - Destroy the objects.
    // Initializes the SDL.
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
    draw(renderer,texture);

    SDL_Surface *resultatSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0, 0, 0, 0);

    reduction_Bruit(surface,resultatSurface);
    applySobel(resultatSurface);
    applySobel(resultatSurface);
    //applyNonMaxSuppression(resultatSurface, surface);
    //applyHysteresis(resultatSurface);

    SDL_SaveBMP(resultatSurface, argv[2]);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(resultatSurface);
    IMG_Quit();
    SDL_Quit();

    return 0;
}*/
