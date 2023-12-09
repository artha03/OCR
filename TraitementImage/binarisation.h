#ifndef BINARISATION_H
#define BINARISATION_H


void calculateHistogram(SDL_Surface* image, double histogram[256]);
int calculateOtsuThreshold(SDL_Surface * surface ,double histogram[256]);
Uint32 pixel_to_bin(Uint32 pixel_color, SDL_PixelFormat* format,int threshold);
void surface_to_bin(SDL_Surface* surface);



#endif