#pragma once
#include "Texture.h"
#include <map>
#include <string>

class TextureManager {
public:
    static Texture* LoadTexture(const std::string&, SDL_Renderer*);
    static void UnloadTexture(const std::string&);
    static Texture* GetTexture(const std::string&);

private:
    static std::map<std::string, Texture*> textures;
};