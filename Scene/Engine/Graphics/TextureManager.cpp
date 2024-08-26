#include "TextureManager.h"

std::map<std::string, Texture*> TextureManager::textures;

Texture* TextureManager::LoadTexture(const std::string& filename, SDL_Renderer* renderer) {
    auto it = textures.find(filename);

    if (it != textures.end()) {
        return it->second;
    }

    Texture* texture = new Texture();
    texture->load(filename, renderer);

    textures[filename] = texture;
    return texture;

}

void TextureManager::UnloadTexture(const std::string& filename) {
    auto it = textures.find(filename);

    if (it != textures.end()) {
        delete it->second;
        textures.erase(it);
    }
}

Texture* TextureManager::GetTexture(const std::string& filename) {
    auto it = textures.find(filename);

    if (it != textures.end()) {
        return it->second;
    }

    return nullptr;
}