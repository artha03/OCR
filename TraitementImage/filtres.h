#ifndef FILTRES_H
#define FILTRES_H

void draw(SDL_Renderer* renderer, SDL_Texture* texture);
void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* change);
SDL_Surface* load_image(const char* path);
void convertToGray(SDL_Surface *image);
void calculateHistogram(SDL_Surface* image, double histogram[256]);
int calculateOtsuThreshold(SDL_Surface * surface ,double histogram[256]);
Uint32 pixel_to_bin(Uint32 pixel_color, SDL_PixelFormat* format,int threshold);
void surface_to_bin(SDL_Surface* surface);
static Uint32 moyenne(SDL_Surface *surface, int i, int j, int n);
void surface_to_flou(SDL_Surface* surface);
Uint8 f(Uint8 c, double n);
void contrast(SDL_Surface * image);
void reduction_Bruit(SDL_Surface *image, SDL_Surface *resultat);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void applySmoothingFilter(SDL_Surface *image);


#endif FILTRES_H