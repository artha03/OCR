#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include "res_sudoku.h"

#define CELL_SIZE 56 // Taille d'une cellule de la grille

void drawText(SDL_Renderer *renderer, TTF_Font *font, int value, int x, int y) {
    SDL_Color color = {0 , 0, 0, 0}; // Couleur blanche

    char text[2];
    if (value== -1)
        sprintf(text, "%d", 0);
    else {
        sprintf(text, "%d", value); // Convertir la valeur en chaîne de caractères
    }
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color); // Créer une surface de texte
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface); // Créer une texture de texte

    SDL_Rect textRect = {x, y, CELL_SIZE, CELL_SIZE}; // Rectangle pour positionner le texte
    SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h); // Obtenir la taille de la texture

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect); // Dessiner le texte sur le rendu

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

int displaySudoku(const char *imagePath, const char *numberFile) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Affichage des chiffres", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur lors de la création du renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("Erreur lors de l'initialisation de SDL_ttf : %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("arial.ttf", 28);
    if (!font) {
        printf("Erreur lors du chargement de la police de caractères : %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *imageSurface = IMG_Load(imagePath);
    if (!imageSurface) {
        printf("Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    if (!imageTexture) {
        printf("Erreur lors de la création de la texture d'image : %s\n", SDL_GetError());
        SDL_FreeSurface(imageSurface);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    FILE *file = fopen(numberFile, "r");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier de chiffres : %s\n", SDL_GetError());
        SDL_DestroyTexture(imageTexture);
        SDL_FreeSurface(imageSurface);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);

    int x = 17;
    int y = 15;
    int value;

    while (fscanf(file, "%d", &value) == 1) {

        drawText(renderer, font, value, x, y);
        x += CELL_SIZE;

        if (x >= 500) {
            x = 17;
            y += CELL_SIZE;
        }

        if (y >= 500) {
            break;
        }
    }

    SDL_RenderPresent(renderer);

   // time_t t = time(NULL);
    //struct tm tm = *localtime(&t);
    char filename[50];
    snprintf(filename, sizeof(filename), "resultat.png");

    SDL_Surface *screenshot = SDL_CreateRGBSurface(0, 500, 500, 32, 0, 0, 0, 0);
    if (!screenshot) {
        printf("Erreur lors de la création de la surface de capture d'écran : %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch) != 0) {
        printf("Erreur lors de la capture d'écran : %s\n", SDL_GetError());
        SDL_FreeSurface(screenshot);
        return 1;
    }

    if (IMG_SavePNG(screenshot, filename) != 0) {
        printf("Erreur lors de l'enregistrement du rendu : %s\n", IMG_GetError());
        SDL_FreeSurface(screenshot);
        return 1;
    }

    fclose(file);
    SDL_DestroyTexture(imageTexture);
    SDL_FreeSurface(imageSurface);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

/*
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Utilisation : %s <fichier_grille_sudoku>\n", argv[0]);
        return 1;
    }

    const char *sudokuFile = argv[1];
    const char *imagePath = "grille_vide.png"; // Chemin de votre image de fond
    const char *numberFile = sudokuFile; // Fichier contenant les chiffres de la grille

    int result = displaySudoku(imagePath, numberFile);

    if (result == 0) {
        printf("Grille de sudoku affichée et enregistrée avec succès !\n");
    } else {
        printf("Une erreur s'est produite lors de l'affichage et de l'enregistrement de la grille de sudoku.\n");
    }

    return result;
}
*/