#pragma once
#include <string>
#include <SDL.h>

class Texture {
public:
    Texture();
    ~Texture();

    void load(const std::string& path, SDL_Renderer* renderer);
    void free();
    void render(SDL_Renderer* renderer, int x, int y, int w = 0, int h = 0, SDL_Rect* clip = nullptr);

private:
    SDL_Texture* texture;
    int width;
    int height;
};