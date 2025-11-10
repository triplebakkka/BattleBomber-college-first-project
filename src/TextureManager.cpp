#include "TextureManager.h"
#include <iostream>

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance() {
    if (!instance) {
        instance = new TextureManager();
    }
    return instance;
}

void TextureManager::DestroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

TextureManager::~TextureManager() {
    UnloadAllTextures();
}

void TextureManager::LoadTexture(const std::string& name, const std::string& filePath) {
    if (textures.find(name) == textures.end()) 
    {
        Texture2D texture = ::LoadTexture(filePath.c_str());
        if (texture.id != 0) 
        {
            textures[name] = texture;
            std::cout << "Loaded texture: " << name << std::endl;
        } 
        else 
        {
            std::cout << "Failed to load texture: " << filePath << std::endl;
        }
    }
}

Texture2D& TextureManager::GetTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    
    // Return a default texture or handle error
    static Texture2D defaultTexture{};
    return defaultTexture;
}

void TextureManager::UnloadTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        ::UnloadTexture(it->second); // call global (raylib) function
        textures.erase(it);
    }
}

void TextureManager::UnloadAllTextures() {
    for (auto& pair : textures) {
        ::UnloadTexture(pair.second); // call global (raylib) function
    }
    textures.clear();
}
