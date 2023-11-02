#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <err.h>




//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////



Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    Uint8 average = 0.333 * r + 0.333 * g + 0.0333 * b;
    
    Uint32 color = SDL_MapRGB(format, average, average, average);

    return color;
}

void surface_to_grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;

    SDL_LockSurface(surface);
    for (int i = 0;i<len;i++)
      {
	pixels[i] = pixel_to_grayscale(pixels[i],format);
      }
    SDL_UnlockSurface(surface);
}

Uint32 pixel_to_bin(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    Uint8 average = 0.3*r + 0.59*g + 0.11*b;

    Uint32 color;

    if(average>=85)
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
    for (int i = 0;i<len;i++)
      {
	pixels[i] = pixel_to_bin(pixels[i],format);
      }
    SDL_UnlockSurface(surface);

}

static Uint32 moyenne(SDL_Surface *surface, int i, int j, int n)
{
    const int initial_h = SDL_max(i - n, 0);
    const int initial_w = SDL_max(j - n, 0);
    const int final_h = SDL_min(i + n, surface->h - 1);
    const int final_w = SDL_min(j + n, surface->w - 1);
    const int nb_pixel = ((final_h - initial_h) * (final_w - initial_w));
    const Uint32 *p = surface->pixels;

    Uint32 sum_r = 0, sum_g = 0, sum_b = 0;
    SDL_Color color;

    for (i = initial_h; i < final_h; i++)
        for(j = initial_w; j < final_w; j++)
        {
            SDL_GetRGB(p[i * surface->w + j], surface->format, &color.r, &color.g, &color.b);
            sum_r += color.r;
            sum_g += color.g;
            sum_b += color.b;
        }

    return SDL_MapRGB(surface->format, sum_r / nb_pixel, sum_g / nb_pixel, sum_b / nb_pixel);
}

void surface_to_flou(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int h = surface->h;
    int w = surface->w;
    //SDL_PixelFormat* format = surface->format;

    SDL_LockSurface(surface);
    for (int i = 0;i<h;i++)
      {
        for (int j =0; j<w;j++)
	  {
	    pixels[i * w + j] = moyenne(surface,i,j,2);
	  }
      }
    SDL_UnlockSurface(surface);

}

// Fonction pour calculer la moyenne mobile sur une image
void reduction_Bruit(SDL_Surface *image, SDL_Surface *resultat) {
    int fenetre = 3; // Taille de la fenêtre de moyenne mobile

    for (int i = 0; i < image->h; i++) {
        for (int j = 0; j < image->w; j++) {
            int sommeR = 0, sommeG = 0, sommeB = 0, pixelCount = 0;

            for (int k = -fenetre / 2; k <= fenetre / 2; k++) {
                for (int l = -fenetre / 2; l <= fenetre / 2; l++) {
                    int x = j + l;
                    int y = i + k;

                    if (x >= 0 && x < image->w && y >= 0 && y < image->h) {
                        Uint32 pixel = ((Uint32 *)image->pixels)[y * image->w + x];
                        sommeR += pixel & 0xFF;
                        sommeG += (pixel >> 8) & 0xFF;
                        sommeB += (pixel >> 16) & 0xFF;
                        pixelCount++;
                    }
                }
            }

            Uint8 avgR = sommeR / pixelCount;
            Uint8 avgG = sommeG / pixelCount;
            Uint8 avgB = sommeB / pixelCount;

            ((Uint32 *)resultat->pixels)[i * resultat->w + j] = avgR | (avgG << 8) | (avgB << 16) | 0xFF000000;
        }
    }
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
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

void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* change,SDL_Window* window)
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
	      SDL_RenderClear(renderer);
	      SDL_Renderer *rendererBlanc = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	      SDL_SetRenderDrawColor(rendererBlanc, 255, 255, 255, 255);
	      draw(rendererBlanc,t);
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
void calculerGradients(SDL_Surface *image, float *gradients) {
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

            gradients[y * w + x] = sqrt(gradientX * gradientX + gradientY * gradientY);
        }
    }
}

// Fonction pour seuiller les contours
void seuillerContours(float *gradients, Uint32 *resultat, float seuil, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int index = y * w + x;

            // Comparer le gradient avec un seuil basé sur la moyenne
            resultat[index] = (gradients[index] > seuil) ? 255 : 0;
        }
    }
}
// Fonction pour supprimer les non-maxima
void supprimerNonMaxima(float *gradients, Uint32 *resultat, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int index = y * w + x;

            // Calculer l'angle du gradient
            float angle = atan2(gradients[index], gradients[index - 1]) * 180.0 / M_PI;

            // Assurer que l'angle est dans la plage [0, 180)
            if (angle < 0) {
                angle += 180.0;
            }

            // Supprimer les non-maxima
            resultat[index] = (gradients[index] >= gradients[index - 1] && gradients[index] >= gradients[index + 1]) ? 255 : 0;
        }
    }
}



int main(int argc,char ** argv) {

  if (argc != 2)
    {
      return EXIT_FAILURE;
    }

  
    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }


    // Créer une fenêtre SDL
    // Creates a window.
    SDL_Window* window = SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0, 0, 0,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a renderer.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(argv[1]);
    if (surface == NULL)
      errx(EXIT_FAILURE,"%s", SDL_GetError());

    int w = surface->w;
    int h = surface->h;

    SDL_SetWindowSize(window, surface->w,surface->h);

    SDL_Texture* texture = IMG_LoadTexture(renderer, argv[1]);
    if (texture == NULL)
      errx(EXIT_FAILURE,"%s", SDL_GetError());
    draw(renderer,texture);


    reduction_Bruit(surface,surface);
    surface_to_grayscale(surface);
    surface_to_flou(surface);
    surface_to_bin(surface);

    // Créer un tableau pour les gradients
    float *gradients = malloc(w * h * sizeof(float));

    // Calculer les gradients
    calculerGradients(surface, gradients);
    // Afficher les valeurs dans le tableau gradients
    //printf("Gradients:\n");
    //for (int y = 0; y < h; y++) {
    //for (int x = 0; x < w; x++) {
    //	int index = y * w + x;
    //  printf("%f ", gradients[index]);
    //}
    //printf("\n");
    //}

    // Créer un tableau pour le résultat après seuillage
    Uint32 *resultatSeuillage = malloc(w * h * sizeof(Uint32));


    // Seuiller les contours avec un seuil de 50
    seuillerContours(gradients, resultatSeuillage, 50.0, w, h);
    
    // Afficher les valeurs dans le tableau resultatSeuillage
    //printf("\nRésultat après seuillage:\n");
    //for (int y = 0; y < h; y++) {
    //for (int x = 0; x < w; x++) {
    //	int index = y * w + x;
    //	printf("%d ", resultatSeuillage[index]);
    //}
    //printf("\n");
    //}

    // Créer un tableau pour le résultat final après suppression des non-maxima
    Uint32 *resultatFinal = malloc(w * h * sizeof(Uint32));

    // Supprimer les non-maxima
    supprimerNonMaxima(gradients, resultatFinal, w, h);
    // Afficher les valeurs dans le tableau resultatFinal
    printf("\nRésultat après suppression des non-maxima:\n");
    for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
    	int index = y * w + x;
    	printf("%d ", resultatFinal[index]);
    }
    printf("\n");
    }

    // Afficher l'image résultante
    //SDL_Surface *resultatSurface = SDL_CreateRGBSurfaceFrom(resultatFinal, surface->w, surface->h, 32, 0, 0, 0, 0);
    SDL_Surface *resultatSurface = SDL_CreateRGBSurfaceFrom(resultatFinal, surface->w, surface->h, 32, surface->w*4,0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    
    SDL_Texture * texture_change = SDL_CreateTextureFromSurface(renderer, resultatSurface);

    // Attendre une touche pour fermer la fenêtre
    // Dispatches the events.
    event_loop(renderer,texture,texture_change,window);

    // Libérer les ressources SDL
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(texture_change);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(resultatSurface);

    // Quitter SDL
    SDL_Quit();

    // Libérer la mémoire des tableaux
    free(gradients);
    free(resultatSeuillage);
    free(resultatFinal);

    return 0;
}
