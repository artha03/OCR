#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SOBEL_THRESHOLD 20
#define HIGH_THRESHOLD 50
#define LOW_THRESHOLD 20

// Fonction de convolution pour le filtre de Sobel
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

// Applique le filtre de Sobel pour détecter les contours
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

// Compare l'intensité du pixel avec ses voisins le long de la direction du gradient
void compareNeighbors(SDL_Surface *gradient, SDL_Surface *image, int x, int y, int dx, int dy) {
    Uint8 *center = (Uint8 *)gradient->pixels + y * gradient->pitch + x * 4;
    Uint8 *neighbor1 = (Uint8 *)gradient->pixels + (y + dy) * gradient->pitch + (x + dx) * 4;
    Uint8 *neighbor2 = (Uint8 *)gradient->pixels + (y - dy) * gradient->pitch + (x - dx) * 4;

    Uint8 *centerPixel = (Uint8 *)image->pixels + y * image->pitch + x * 4;
    Uint8 *neighbor1Pixel = (Uint8 *)image->pixels + (y + dy) * image->pitch + (x + dx) * 4;
    Uint8 *neighbor2Pixel = (Uint8 *)image->pixels + (y - dy) * image->pitch + (x - dx) * 4;

    if (center[0] < neighbor1[0] || center[0] < neighbor2[0]) {
        centerPixel[0] = centerPixel[1] = centerPixel[2] = 0; // Suppression du faux positif
    } else {
        centerPixel[0] = centerPixel[1] = centerPixel[2] = 255; // Contour potentiel
    }
}

// Suppression des faux positifs (non-maxima suppression)
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


// Applique l'hystérésis pour connecter les contours
void applyHysteresis(SDL_Surface *image) {
    for (int y = 1; y < image->h - 1; y++) {
        for (int x = 1; x < image->w - 1; x++) {
            Uint8 *pixel = (Uint8 *)image->pixels + y * image->pitch + x * 4;
            if (pixel[0] == 128) {
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        Uint8 *neighbor = (Uint8 *)image->pixels + (y + i) * image->pitch + (x + j) * 4;
                        if (neighbor[0] == 255) {
                            pixel[0] = pixel[1] = pixel[2] = 255; // Contour connecté
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_image.jpg output_image.jpg\n", argv[0]);
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    SDL_Surface *image = IMG_Load(argv[1]);

    if (!image) {
        printf("Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface *gradient = SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);
    applySobel(image);
    applyNonMaxSuppression(gradient, image);
    applyHysteresis(image);

    SDL_SaveBMP(image, argv[2]); // Le dernier paramètre (100) est la qualité de compression

    SDL_FreeSurface(image);
    SDL_FreeSurface(gradient);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
