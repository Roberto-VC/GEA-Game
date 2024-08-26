#include "Texture.h"
#include <SDL_render.h>
#include <SDL_surface.h>


Texture::Texture()
  : texture(nullptr), width(0), height(0) {
}

Texture::~Texture() {
    free();
}

void Texture::load(const std::string& path, SDL_Renderer* renderer) {
    free();
    SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());

    texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

    width = loadedSurface->w;
    height = loadedSurface->h;

    /* SDL_FreeSurface(loadedSurface); */
}

void Texture::free() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }
}

void Texture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip) {

    SDL_Rect srcrect = SDL_Rect{ x, y, width, height };

    if (clip != nullptr) {
        srcrect.w = clip->w;
        srcrect.h = clip->h;
    }

    SDL_RenderCopy(renderer, texture, &srcrect, clip);
}