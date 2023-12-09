#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// Fonction pour calculer l'histogramme d'une image en niveaux de gris
void calculateHistogram(SDL_Surface* image, double histogram[256])
{
    SDL_LockSurface(image);

    const int totalPixels = image->w * image->h;

    for (int i = 0; i < 256; ++i) {
        histogram[i] = 0.0;
    }

    for (int y = 0; y < image->h; ++y) {
        for (int x = 0; x < image->w; ++x) {
            Uint8 pixelValue = ((Uint8*)image->pixels)[y * image->pitch + x];
            histogram[pixelValue]++;
        }
    }

    for (int i = 0; i < 256; ++i) {
        histogram[i] /= totalPixels;
    }

    SDL_UnlockSurface(image);
}

// Fonction pour calculer la valeur du seuil d'Otsu
int calculateOtsuThreshold(SDL_Surface * surface ,double histogram[256])
{
    const int numBins = 256;
    const int totalPixels = surface->w * surface->h; // Remplacez ces valeurs par les dimensions de votre image

    double sum = 0.0;
    for (int i = 0; i < numBins; ++i) {
        sum += i * histogram[i];
    }

    double sumB = 0.0;
    double wB = 0.0;
    double wF = 0.0;

    double varMax = 0.0;
    int threshold = 0;

    for (int i = 0; i < numBins; ++i) {
        wB += histogram[i];
        if (wB == 0.0)
            continue;

        wF = 1.0 - wB;
        if (wF == 0.0)
            break;

        sumB += i * histogram[i];

        double mB = sumB / (wB * totalPixels);
        double mF = (sum - sumB) / (wF * totalPixels);

        double varBetween = wB * wF * (mB - mF) * (mB - mF);

        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = i;
        }
    }

    return threshold;
}

Uint32 pixel_to_bin(Uint32 pixel_color, SDL_PixelFormat* format,int threshold)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    //printf("R = %d, G = %d, B = %d",r, g, b);

    Uint8 average = 0.3*r + 0.59*g + 0.11*b;

    Uint32 color;

    if(average>=threshold)
    {
        color = SDL_MapRGB(format,0, 0, 0);
    }
    else
    {
        color = SDL_MapRGB(format, 255,255, 255);
    }

    return color;
}

void surface_to_bin(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;

    SDL_LockSurface(surface);

    double histogram[256] = {0};
    // Calcul de l'histogramme initial
    calculateHistogram(surface, histogram);

    int threshold = calculateOtsuThreshold(surface,histogram);

    for (int i = 0;i<len;i++)
    {
        pixels[i] = pixel_to_bin(pixels[i],format,threshold);
    }
    SDL_UnlockSurface(surface);

}