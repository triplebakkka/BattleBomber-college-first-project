#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"

class Bullet {
private:
    Vector2 position;
    Vector2 velocity;
    float speed;
    bool shouldDestroy;
    bool hasPowerUp;
    
public:
    Bullet(Vector2 startPos, Vector2 direction, bool powerUp = false);
    void Update();
    void Draw();
    bool ShouldDestroy() const;
    Vector2 GetPosition() const;
    bool HasPowerUp() const;
    void MarkForDestruction();
};

#endif
