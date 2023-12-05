#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
/*
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
}*/

void convertToGray(SDL_Surface *image) {
    for (int y = 0; y < image->h; ++y) {
        for (int x = 0; x < image->w; ++x) {
            Uint32 pixel = *((Uint32*)image->pixels + y * image->w + x);
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);
            Uint8 gray = 0.3 * r + 0.59 * g + 0.11 * b;
            *((Uint32*)image->pixels + y * image->w + x) = SDL_MapRGBA(image->format, gray, gray, gray, a);
        }
    }
}

/*
double calculateEntropy(int histogram[], int start, int end) {
    double entropy = 0.0;
    int totalPixels = 0;

    for (int i = start; i <= end; ++i) {
        totalPixels += histogram[i];
    }

    for (int i = start; i <= end; ++i) {
        double probability = (double)histogram[i] / totalPixels;
        if (probability > 0.0) {
            entropy -= probability * log2(probability);
        }
    }

    return entropy;
}

int findAdaptiveThreshold(SDL_Surface *surface, int windowSize) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    int histogram[256] = {0};
    int totalPixels = width * height;

    // Calcul de l'histogramme initial
    for (int i = 0; i < totalPixels; ++i) {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);
        int gray = (int)(0.3 * r + 0.59 * g + 0.11 * b);
        histogram[gray]++;
    }

    int adaptiveThreshold = 0;

    // Calcul du seuil initial
    for (int i = 0; i < 256; ++i) {
        adaptiveThreshold += i * histogram[i];
    }
    adaptiveThreshold /= totalPixels;

    // Binarisation basée sur l'entropie
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int sum = 0;
            int count = 0;

            for (int i = -windowSize; i <= windowSize; ++i) {
                for (int j = -windowSize; j <= windowSize; ++j) {
                    int nx = x + i;
                    int ny = y + j;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        Uint8 r, g, b;
                        SDL_GetRGB(pixels[ny * width + nx], surface->format, &r, &g, &b);
                        int gray = (int)(0.3 * r + 0.59 * g + 0.11 * b);
                        sum += gray;
                        count++;
                    }
                }
            }

            int localMean = sum / count;

            double entropyForeground = calculateEntropy(histogram, 0, localMean);
            double entropyBackground = calculateEntropy(histogram, localMean + 1, 255);

            // Calcul du seuil basé sur l'entropie
            adaptiveThreshold = (int)((entropyForeground + entropyBackground) / 0.06);
        }
    }

    return adaptiveThreshold;
}*/
////////////
// Fonction pour calculer l'histogramme d'une image en niveaux de gris
void calculateHistogram(SDL_Surface* image, double histogram[256])
{
    SDL_LockSurface(image);

    const int totalPixels = image->w * image->h;

    for (int i = 0; i < 256; ++i) {
        histogram[i] = 0.0;
    }

    for (int y = 0; y < image->h; ++y) {
        for (int x = 0; x < image->w; ++x) {
            Uint8 pixelValue = ((Uint8*)image->pixels)[y * image->pitch + x];
            histogram[pixelValue]++;
        }
    }

    for (int i = 0; i < 256; ++i) {
        histogram[i] /= totalPixels;
    }

    SDL_UnlockSurface(image);
}

// Fonction pour calculer la valeur du seuil d'Otsu
int calculateOtsuThreshold(SDL_Surface * surface ,double histogram[256])
{
    const int numBins = 256;
    const int totalPixels = surface->w * surface->h; // Remplacez ces valeurs par les dimensions de votre image

    double sum = 0.0;
    for (int i = 0; i < numBins; ++i) {
        sum += i * histogram[i];
    }

    double sumB = 0.0;
    double wB = 0.0;
    double wF = 0.0;

    double varMax = 0.0;
    int threshold = 0;

    for (int i = 0; i < numBins; ++i) {
        wB += histogram[i];
        if (wB == 0.0)
            continue;

        wF = 1.0 - wB;
        if (wF == 0.0)
            break;

        sumB += i * histogram[i];

        double mB = sumB / (wB * totalPixels);
        double mF = (sum - sumB) / (wF * totalPixels);

        double varBetween = wB * wF * (mB - mF) * (mB - mF);

        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = i;
        }
    }

    return threshold;
}

Uint32 pixel_to_bin(Uint32 pixel_color, SDL_PixelFormat* format,int threshold)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    //printf("R = %d, G = %d, B = %d",r, g, b);

    Uint8 average = 0.3*r + 0.59*g + 0.11*b;

    Uint32 color;

    if(average>=threshold)
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

    double histogram[256] = {0};
    // Calcul de l'histogramme initial
    calculateHistogram(surface, histogram);

    int threshold = calculateOtsuThreshold(surface,histogram);

    for (int i = 0;i<len;i++)
      {
	    pixels[i] = pixel_to_bin(pixels[i],format,threshold);
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
	    pixels[i * w + j] = moyenne(surface,i,j,3);
	  }
      }
    SDL_UnlockSurface(surface);
}

Uint8 f(Uint8 c, double n)
{
    if(c <= 255 / 2)
        return (Uint8)( (255/2) * SDL_pow((double) 2 * c / 255, n));
    else
        return 255 - f(255 - c, n);
}

void contrast(SDL_Surface * image)
{
    Uint8 * pixels = image->pixels;
    int len = image->w * image->h;
    //SDL_PixelFormat* format = image->format;

    SDL_LockSurface(image);


    for (int i = 0;i<len*4;i++)
    {
        pixels[i] = f(pixels[i],3);
    }
    SDL_UnlockSurface(image);
}

void reduction_Bruit(SDL_Surface *image, SDL_Surface *resultat) {
    int fenetre = 3;

    for (int i = 0; i < image->h; i++)
    {
        for (int j = 0; j < image->w; j++)
        {
            int sommeR = 0;
            int sommeG = 0;
            int sommeB = 0;
            int pixelCount = 0;

            for (int k = -fenetre / 2; k <= fenetre / 2; k++)
            {
                for (int l = -fenetre / 2; l <= fenetre / 2; l++)
                {
                    int posx = j + l;
                    int posy = i + k;

                    if (posx >= 0 && posx < image->w && posy >= 0 && posy < image->h)
                    {
                        Uint32 pixel = ((Uint32 *)image->pixels)[posy * image->w + posx];
                        sommeR += pixel %256;
                        sommeG += (pixel >> 8) %256;
                        sommeB += (pixel >> 16) %256;
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

/*
void applySharpenFilter(SDL_Surface *image) {
    if (SDL_LockSurface(image) == 0) {
        Uint32 *pixels = (Uint32 *)image->pixels;
        int pitch = image->pitch / sizeof(Uint32);
        int width = image->w;
        int height = image->h;

        // Créer une copie temporaire de l'image
        Uint32 *tempPixels = malloc(image->pitch * image->h);
        memcpy(tempPixels, pixels, image->pitch * image->h);

        // Appliquer le filtre de netteté
        for (int y = 1; y < height - 1; ++y) {
            for (int x = 1; x < width - 1; ++x) {
                Uint8 *center = (Uint8 *)&tempPixels[y * pitch + x];
                Uint8 *up = (Uint8 *)&pixels[(y - 1) * pitch + x];
                Uint8 *down = (Uint8 *)&pixels[(y + 1) * pitch + x];
                Uint8 *left = (Uint8 *)&pixels[y * pitch + x - 1];
                Uint8 *right = (Uint8 *)&pixels[y * pitch + x + 1];

                for (int i = 0; i < 3; ++i) {
                    center[i] = center[i] + 5 * center[i] - up[i] - down[i] - left[i] - right[i];

                    if (center[i] > 255) {
                        center[i] = 255;
                    } else if (center[i] < 0) {
                        center[i] = 0;
                    }
                }
            }
        }

        // Copier les pixels modifiés vers l'image d'origine
        memcpy(pixels, tempPixels, image->pitch * image->h);

        SDL_UnlockSurface(image);
        free(tempPixels);
    }
}*/

// Déclaration de la fonction pour obtenir la couleur d'un pixel
Uint32 getpixel(SDL_Surface *surface, int x, int y);

// Déclaration de la fonction pour définir la couleur d'un pixel
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

// Fonction pour appliquer un filtre de lissage à l'image
void applySmoothingFilter(SDL_Surface *image) {
    int width = image->w;
    int height = image->h;

    SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_BlitSurface(image, NULL, tempSurface, NULL);

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            // Apply simple smoothing filter
            int r = 0, g = 0, b = 0;

            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    Uint8 tr, tg, tb;
                    SDL_GetRGB(getpixel(tempSurface, x + i, y + j), tempSurface->format, &tr, &tg, &tb);
                    r += tr;
                    g += tg;
                    b += tb;
                }
            }

            r /= 9;
            g /= 9;
            b /= 9;

            // S'assurer que les valeurs sont dans la plage 0-255
            r = (r < 0) ? 0 : (r > 255) ? 255 : r;
            g = (g < 0) ? 0 : (g > 255) ? 255 : g;
            b = (b < 0) ? 0 : (b > 255) ? 255 : b;

            // Set the pixel with the smoothed values
            putpixel(tempSurface, x, y, SDL_MapRGB(tempSurface->format, r, g, b));
        }
    }

    SDL_BlitSurface(tempSurface, NULL, image, NULL);
    SDL_FreeSurface(tempSurface);
}

// Définition de la fonction pour obtenir la couleur d'un pixel
Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    return *(Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
}

// Définition de la fonction pour définir la couleur d'un pixel
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    *(Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel) = pixel;
}


/*
void applySharpnessFilter(SDL_Surface *image) {
    if (SDL_LockSurface(image) != 0) {
        SDL_Log("Unable to lock surface: %s\n", SDL_GetError());
        return;
    }

    int width = image->w;
    int height = image->h;
    Uint32 *pixels = (Uint32 *)image->pixels;

    Uint32 *newPixels = malloc(width * height * sizeof(Uint32));
    if (newPixels == NULL) {
        SDL_Log("Memory allocation failed\n");
        SDL_UnlockSurface(image);
        return;
    }

    // Appliquer le filtre de netteté
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            // Noyau de filtre de netteté simple avec pondération pour atténuer le bruit
            int sum = 5 * ((pixels[y * width + x] >> 16) & 0xFF) -
                      ((pixels[(y - 1) * width + x] >> 16) & 0xFF) -
                      ((pixels[(y + 1) * width + x] >> 16) & 0xFF) -
                      ((pixels[y * width + x - 1] >> 16) & 0xFF) -
                      ((pixels[y * width + x + 1] >> 16) & 0xFF);

            // Ajouter une pondération pour atténuer le bruit
            int originalRed = (pixels[y * width + x] >> 16) & 0xFF;
            sum = originalRed + 0.6 * sum;
            sum = sum < 0 ? 0 : (sum > 255 ? 255 : sum);

            // Mettre à jour le pixel dans la nouvelle image
            newPixels[y * width + x] = (pixels[y * width + x] & 0xFF000000) | (sum << 16) | (sum << 8) | sum;
        }
    }

    // Copier les pixels modifiés dans l'image d'origine
    memcpy(pixels, newPixels, width * height * sizeof(Uint32));

    SDL_UnlockSurface(image);
    free(newPixels);
}*/

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 3)
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

    //surface_to_grayscale(resultatSurface);
    //surface_to_flou(resultatSurface);

    //applySharpnessFilter(surface);
    //contrast(surface);

    //applySmoothingFilter(surface);
    reduction_Bruit(surface,resultatSurface);
    //contrast(resultatSurface);
    convertToGray(resultatSurface);


    surface_to_bin(resultatSurface);

    
    SDL_Texture * texture_change = SDL_CreateTextureFromSurface(renderer, resultatSurface);

    SDL_SaveBMP(resultatSurface, argv[2]);

    // Dispatches the events.
    event_loop(renderer,texture,texture_change);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(resultatSurface);
    


    // Destroys the objects.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture_change);
    SDL_DestroyTexture(texture);
    //SDL_DestroySurface(surface);
    SDL_Quit();

    return EXIT_SUCCESS;
}
