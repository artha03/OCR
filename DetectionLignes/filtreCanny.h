#ifndef FILTRECANNY_H
#define FILTRECANNY_H

void draw(SDL_Renderer* renderer, SDL_Texture* texture);
SDL_Surface* load_image(const char* path);
double convolution(SDL_Surface *image, int x, int y, int kernel[3][3]);
void applySobel(SDL_Surface *image);

#endif