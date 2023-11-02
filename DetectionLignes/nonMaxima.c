#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <err.h>



void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}


// path: Path of the image.
SDL_Surface* load_image(const char* path)
{
    SDL_Surface * image = IMG_Load(path);
    if (image == NULL)
      errx(EXIT_FAILURE,"%s", SDL_GetError());
    SDL_Surface * image_ = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(image);
    return image_;
}


void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* change)
{
    SDL_Event event;
    SDL_Texture* t = colored;

    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
            // If the "quit" button is pushed, ends the event loop.
            case SDL_QUIT:
                return;

            // If the window is resized, updates and redraws the diagonals.
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    draw(renderer, t);
                }
                break;
	case SDL_KEYDOWN:
	  if (t == colored)
	    {
	      t = change;
	      draw(renderer,t);
	      break;
	    }
	  else
	    {
	      t = colored;
	      draw(renderer,t);
	      break;
	    }
	  
        }
    }
}



// Fonction pour calculer les gradients avec l'opérateur de Sobel
void calculerGradients(SDL_Surface *image, float *gradientsX, float *gradientsY) {
    int w = image->w;
    int h = image->h;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            Uint8 *pixelUpLeft = (Uint8 *)image->pixels + (y - 1) * image->pitch + (x - 1);
            Uint8 *pixelUp = pixelUpLeft + image->pitch;
            Uint8 *pixelUpRight = pixelUp + image->pitch;
            Uint8 *pixelLeft = pixelUpLeft + 1;
            Uint8 *pixelRight = pixelUpRight + 1;
            Uint8 *pixelDownLeft = pixelLeft + image->pitch;
            Uint8 *pixelDown = pixelDownLeft + image->pitch;
            Uint8 *pixelDownRight = pixelDown + 1;

            float gradientX = (*pixelUpRight + 2 * *pixelRight + *pixelDownRight) -
                              (*pixelUpLeft + 2 * *pixelLeft + *pixelDownLeft);

            float gradientY = (*pixelDownLeft + 2 * *pixelDown + *pixelDownRight) -
                              (*pixelUpLeft + 2 * *pixelUp + *pixelUpRight);

            gradientsX[y * w + x] = gradientX;
            gradientsY[y * w + x] = gradientY;
        }
    }
}

// Fonction pour supprimer les non-maxima
void supprimerNonMaxima(float *gradientsX, float *gradientsY, Uint8 *resultat, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int index = y * w + x;
            float gradientX = gradientsX[index];
            float gradientY = gradientsY[index];

            // Calculer l'angle du gradient
            float angle = atan2(gradientY, gradientX) * 180.0 / M_PI;

            // Assurer que l'angle est dans la plage [0, 180)
            if (angle < 0) {
                angle += 180.0;
            }

            // Supprimer les non-maxima
            resultat[index] = 255; // Mettre à 255 par défaut

            // Comparer avec les pixels voisins dans la direction du gradient
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
                if (gradientsX[index] < gradientsX[index - 1] || gradientsX[index] < gradientsX[index + 1]) {
                    resultat[index] = 0;
                }
            } else if (angle >= 22.5 && angle < 67.5) {
                if (gradientsX[index] < gradientsX[index - w + 1] || gradientsX[index] < gradientsX[index + w - 1]) {
                    resultat[index] = 0;
                }
            } else if (angle >= 67.5 && angle < 112.5) {
                if (gradientsX[index] < gradientsX[index - w] || gradientsX[index] < gradientsX[index + w]) {
                    resultat[index] = 0;
                }
            } else if (angle >= 112.5 && angle < 157.5) {
                if (gradientsX[index] < gradientsX[index - w - 1] || gradientsX[index] < gradientsX[index + w + 1]) {
                    resultat[index] = 0;
                }
            }
        }
    }
}

int main(int argc, char ** argv) {

  if (argc != 2)
    {
      return EXIT_FAILURE;
    }

  
    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    // Charger l'image avec SDL_image
    SDL_Surface *imageSurface = load_image(argv[1]);
    if (!imageSurface) {
        printf("SDL_image could not load the image: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    int w = imageSurface->w;
    int h = imageSurface->h;

    // Créer une fenêtre SDL
    SDL_Window *window = SDL_CreateWindow("Suppression des non-maxima", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);

    // Créer des tableaux pour les gradients X et Y
    float *gradientsX = malloc(w * h * sizeof(float));
    float *gradientsY = malloc(w * h * sizeof(float));

    // Calculer les gradients
    calculerGradients(imageSurface, gradientsX, gradientsY);

    // Créer un tableau pour le résultat
    Uint8 *resultat = malloc(w * h * sizeof(Uint8));

    // Supprimer les non-maxima
    supprimerNonMaxima(gradientsX, gradientsY, resultat, w, h);

    // Libérer les ressources SDL
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(imageSurface);

    // Quitter SDL
    SDL_Quit();

    // Libérer la mémoire des tableaux
    free(gradientsX);
    free(gradientsY);
    free(resultat);

    return 0;
}
