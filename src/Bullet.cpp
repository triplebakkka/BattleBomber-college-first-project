#include "Bullet.h"
#include "TextureManager.h"
#include <math.h>

Bullet::Bullet(Vector2 startPos, Vector2 direction, bool powerUp) 
    : position{startPos}, velocity{direction}, speed(5.0f), 
      shouldDestroy(false), hasPowerUp(powerUp) {}

void Bullet::Update() {
    position.x += velocity.x * speed;
    position.y += velocity.y * speed;
    
    // Check if bullet is out of bounds
    if (position.x < 0 || position.x > 800 || position.y < 0 || position.y > 600) {
        shouldDestroy = true;
    }
}

void Bullet::Draw() {
    DrawDebug(false);
}

void Bullet::DrawDebug(bool debugMode) {
    if (debugMode) {
        // Draw hitbox instead of sprite
        Rectangle hitbox = GetHitbox();
        Color hitboxColor = hasPowerUp ? YELLOW : RED;
        DrawRectangleRec(hitbox, hitboxColor);
        DrawRectangleLinesEx(hitbox, 1.0f, BLACK);
    } else {
        // Draw sprite normally
        TextureManager* texManager = TextureManager::GetInstance();
        Texture2D bulletTexture = texManager->GetTexture("bullet");
        
        // Calculate rotation based on velocity
        float rotation = atan2(velocity.y, velocity.x) * RAD2DEG;
        
        Rectangle sourceRect = {0, 0, (float)bulletTexture.width, (float)bulletTexture.height};
        Rectangle destRect = {position.x, position.y, 8, 8}; // Adjust size as needed
        Vector2 origin = {4, 4}; // Half of the destRect size
        
        Color tint = hasPowerUp ? YELLOW : WHITE;
        DrawTexturePro(bulletTexture, sourceRect, destRect, origin, rotation, tint);
    }
}

bool Bullet::ShouldDestroy() const {
    return shouldDestroy;
}

Vector2 Bullet::GetPosition() const {
    return position;
}

Rectangle Bullet::GetHitbox() const {
    // Bullet hitbox: 8x8 centered on position
    return {position.x - 4, position.y - 4, 8, 8};
}

bool Bullet::HasPowerUp() const {
    return hasPowerUp;
}

void Bullet::MarkForDestruction() {
    shouldDestroy = true;
}
