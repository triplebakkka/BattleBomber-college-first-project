#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "raylib.h"
#include <unordered_map>
#include <string>

class TextureManager {
private:
    std::unordered_map<std::string, Texture2D> textures;
    static TextureManager* instance;

    TextureManager() = default;
    ~TextureManager();

public:
    static TextureManager* GetInstance();
    static void DestroyInstance();

    void LoadTexture(const std::string& name, const std::string& filePath);
    Texture2D& GetTexture(const std::string& name);
    void UnloadTexture(const std::string& name);
    void UnloadAllTextures();
};

#endif
