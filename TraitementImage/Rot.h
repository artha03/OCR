#ifndef ROT_H
#define ROT_H


//void draw(SDL_Renderer* renderer, SDL_Texture* texture);
//void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* grayscale, SDL_Texture* flou,SDL_Texture* binarisation);
//SDL_Surface* load_image(const char* path);
void perform_rotation(SDL_Renderer* renderer, double angle_rotation, SDL_Texture* texture, int w, int h, int new_width, int new_height, int c);


#endif
