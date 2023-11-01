#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}

// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* grayscale)
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
	      t = grayscale;
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

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
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


// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.


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
	    pixels[i * w + j] = moyenne(surface,i,j,1);
	  }
      }
    SDL_UnlockSurface(surface);

    //int w= surface->w;
    //int h = surface->h;

    //for(int i = 0; i < h; i++)
    // {
    //for(int j = 0; j < w; j++)
    //	  {
    //	    Uint32 color = SDL_GetRGB(pixels[i*w+j, s->format, &r, &g, &b);
    //	    pixels[i * w + j] = SDL_MapRGB(s->format, 255 - color.r, 255 - color.g, 255 - color.b);
    //	  }
    //}

}

int main(int argc, char** argv)
{
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

    surface_to_flou(surface);

    SDL_Texture * texture_flou = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
    
    // Dispatches the events.
    event_loop(renderer,texture,texture_flou);

    // Destroys the objects.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture_flou);
    SDL_DestroyTexture(texture);
    //SDL_DestroySurface(surface);
    SDL_Quit();

    return EXIT_SUCCESS;
}
