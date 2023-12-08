#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct {
    int x1, y1; // Coin supérieur gauche
    int x2, y2; // Coin supérieur droit
    int x3, y3; // Coin inférieur gauche
    int x4, y4; // Coin inférieur droit
} RegionCoordinates;

void captureRegion(const char *inputPath, const char *outputPath, const RegionCoordinates *coordinates) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Capture d'écran", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Erreur lors de la création de la fenêtre SDL : %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Erreur lors de la création du rendu SDL : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    SDL_Delay(100);

    // Charger l'image depuis le chemin d'entrée
    SDL_Texture *imageTexture = IMG_LoadTexture(renderer, inputPath);
    if (!imageTexture) {
        fprintf(stderr, "Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Capturer la région spécifiée par les coordonnées
    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Surface *screenshot = SDL_CreateRGBSurface(0, SDL_GetWindowSurface(window)->w, SDL_GetWindowSurface(window)->h, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

    SDL_Surface *regionSurface = SDL_CreateRGBSurface(0, abs(coordinates->x2 - coordinates->x1), abs(coordinates->y3 - coordinates->y1), 32, 0, 0, 0, 0);
    SDL_Rect sourceRect = { coordinates->x1, coordinates->y1, regionSurface->w, regionSurface->h };
    SDL_BlitSurface(screenshot, &sourceRect, regionSurface, NULL);

    IMG_SavePNG(regionSurface, outputPath);

    // Libération des ressources
    SDL_DestroyTexture(imageTexture);
    SDL_FreeSurface(screenshot);
    SDL_FreeSurface(regionSurface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <chemin_image_entree> <chemin_image_sortie>\n", argv[0]);
        return 1;
    }

    RegionCoordinates sudokuCoordinates = { 200, 200, 300, 500, 200, 600, 700, 700 };

    captureRegion(argv[1], argv[2], &sudokuCoordinates);

    return 0;
}
