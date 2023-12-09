#ifndef RES_SUDOKU_H
#define RES_SUDOKU_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

void drawText(SDL_Renderer *renderer, TTF_Font *font, int value, int x, int y);
int displaySudoku(const char *imagePath, const char *numberFile);

#endif