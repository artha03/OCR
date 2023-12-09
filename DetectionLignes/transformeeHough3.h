
#ifndef TRANSFORMEEHOUGH3_H
#define TRANSFORMEEHOUGH3_H

typedef struct
{
    double rho;
    double theta;
    int direction; // 0:horizontal    1:vertical
} Line;

Uint8 *pixelref(SDL_Surface *surface, int posx, int posy);
Uint32 SDL_GetPixel(SDL_Surface *surface, int posx, int posy);
void SDL_DrawLine(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 color);
void houghTransform(SDL_Surface *cannyImage);

#endif
