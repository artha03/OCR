#ifndef GRAYSCALE_H
#define GRAYSCALE_H


void draw(SDL_Renderer* renderer, SDL_Texture* texture);
void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* grayscale, SDL_Texture* flou,SDL_Texture* binarisation);
SDL_Surface* load_image(const char* path);
void convertToGray(SDL_Surface *image);


#endif

