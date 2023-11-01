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


void applyKuwaharaFilter(SDL_Surface *inputSurface, SDL_Surface *outputSurface, int x, int y) {
    int radii[4] = {WINDOW_SIZE / 2, (WINDOW_SIZE / 2) + 1, (WINDOW_SIZE / 2) - 1, WINDOW_SIZE / 2};

    for (int i = 0; i < 4; i++) {
        int radius = radii[i];
        int count[4] = {0};
        int sum[4] = {0};

        for (int dx = -radius; dx <= radius; dx++) {
            for (int dy = -radius; dy <= radius; dy++) {
                int nx = x + dx;
                int ny = y + dy;

                if (nx >= 0 && nx < inputSurface->w && ny >= 0 && ny < inputSurface->h) {
                    Uint32 pixel = *((Uint32 *)inputSurface->pixels + ny * inputSurface->w + nx);

                    Uint8 r, g, b, a;
                    SDL_GetRGBA(pixel, inputSurface->format, &r, &g, &b, &a);

                    sum[0] += r;
                    sum[1] += g;
                    sum[2] += b;
                    sum[3] += a;

                    count[0]++;
                    count[1]++;
                    count[2]++;
                    count[3]++;
                }
            }
        }

        int minVarianceIndex = 0;
        int minVariance = INT_MAX;

        for (int j = 0; j < 4; j++) {
            int mean = sum[j] / count[j];
            int variance = 0;

            for (int dx = -radius; dx <= radius; dx++) {
                for (int dy = -radius; dy <= radius; dy++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < inputSurface->w && ny >= 0 && ny < inputSurface->h) {
                        Uint32 pixel = *((Uint32 *)inputSurface->pixels + ny * inputSurface->w + nx);

                        Uint8 r, g, b, a;
                        SDL_GetRGBA(pixel, inputSurface->format, &r, &g, &b, &a);

                        switch (j) {
                            case 0: variance += (r - mean) * (r - mean); break;
                            case 1: variance += (g - mean) * (g - mean); break;
                            case 2: variance += (b - mean) * (b - mean); break;
                            case 3: variance += (a - mean) * (a - mean); break;
                        }
                    }
                }
            }

            if (variance < minVariance) {
                minVariance = variance;
                minVarianceIndex = j;
            }
        }

        Uint32 *outputPixel = ((Uint32 *)outputSurface->pixels) + y * outputSurface->w + x;
        Uint32 inputPixel = *((Uint32 *)inputSurface->pixels + y * inputSurface->w + x);

        Uint8 r, g, b, a;
        SDL_GetRGBA(inputPixel, inputSurface->format, &r, &g, &b, &a);

        switch (minVarianceIndex) {
            case 0: *outputPixel = SDL_MapRGBA(outputSurface->format, mean, g, b, a); break;
            case 1: *outputPixel = SDL_MapRGBA(outputSurface->format, r, mean, b, a); break;
            case 2: *outputPixel = SDL_MapRGBA(outputSurface->format, r, g, mean, a); break;
            case 3: *outputPixel = SDL_MapRGBA(outputSurface->format, r, g, b, mean); break;
        }
    }
}

// Appliquer le filtre de Kuwahara à l'ensemble de l'image
void kuwaharaFilter(SDL_Surface *inputSurface, SDL_Surface *outputSurface) {
    for (int x = 0; x < inputSurface->w; x++) {
        for (int y = 0; y < inputSurface->h; y++) {
            applyKuwaharaFilter(inputSurface, outputSurface, x, y);
        }
    }
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
