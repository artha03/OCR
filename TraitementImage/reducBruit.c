#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
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


// Fonction pour calculer la moyenne mobile sur une image
void moyenneMobileImage(SDL_Surface *image, SDL_Surface *resultat) {
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


int main(int argc,char** argv) {
      // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    // TODO:
    // - Initialize the SDL.
    // - Create a window.
    // - Create a renderer.
    // - Create a surface from the colored image.
    // - Resize the window according to the size of the image.
    // - Create a texture from the colored surface.
    // - Convert the surface into grayscale.
    // - Create a new texture from the grayscale surface.
    // - Free the surface.
    // - Dispatch the events.
    // - Destroy the objects.
    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

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


    SDL_SetWindowSize(window, surface->w,surface->h);

    SDL_Texture* texture = IMG_LoadTexture(renderer, argv[1]);
    if (texture == NULL)
      errx(EXIT_FAILURE,"%s", SDL_GetError());
    draw(renderer,texture);


    SDL_Surface *resultatSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0, 0, 0, 0);
    moyenneMobileImage(surface,resultatSurface);
    
    SDL_Texture * texture_change = SDL_CreateTextureFromSurface(renderer, resultatSurface);
    SDL_FreeSurface(surface);
    
    // Dispatches the events.
    event_loop(renderer,texture,texture_change);

    // Destroys the objects.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture_change);
    SDL_DestroyTexture(texture);
    //SDL_DestroySurface(surface);
    SDL_Quit();

    return EXIT_SUCCESS;
}
