#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void perform_rotation(SDL_Renderer* renderer, double angle_rotation, SDL_Texture* texture, int w, int h, int new_width, int new_height, int c)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect position;
    SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
    position.x = w / 2 - position.w / 2;
    position.y = h / 2 - position.h / 2;

    SDL_Point center = {position.w / 8, position.h / 8};

    SDL_Rect destinationRect;
    destinationRect.x = c-position.w/8;//w / 2 - new_width / 2;
    destinationRect.y = c-position.h/8;//h / 2 - new_height / 2;
    destinationRect.w = new_width/4;
    destinationRect.h = new_height/4;

    SDL_RenderCopyEx(renderer, texture, NULL, &destinationRect, angle_rotation, &center, SDL_FLIP_NONE);

    SDL_RenderPresent(renderer);
    //c = center;
}

void event_loop(SDL_Renderer* renderer, SDL_Texture* texture, int w, int h, int new_width, int new_height, double angle_rotation,int c)
{
    SDL_Event event;
    double angle = 0;

    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                return;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    draw(renderer, texture);
                }
                break;

            case SDL_KEYDOWN:
                angle += angle_rotation;
                if (angle >= 360)
                    angle -= 360;
                perform_rotation(renderer, angle, texture, w, h, new_width, new_height,c);
                break;
        }
    }
}

SDL_Surface* load_image(const char* path)
{
    SDL_Surface * image = IMG_Load(path);
    if (image == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface * image_ = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(image);
    return image_;
}

int main(int argc, char** argv)
{
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: image-file angle");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Rotating Resizable Image", 0, 0, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
int c = 400;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(argv[1]);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    int w = surface->w;
    int h = surface->h;

    SDL_SetWindowSize(window, w, h);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    draw(renderer, texture);

    double angle_rotation = atof(argv[2]);

    event_loop(renderer, texture, w, h, w, h, angle_rotation,c);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
