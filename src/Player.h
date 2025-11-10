#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "Bullet.h"
#include <vector>

class Player {
private:
    Vector2 position;
    Vector2 size;
    Color color;
    float speed;
    Vector2 direction;
    std::vector<Bullet> bullets;
    float fireCooldown;
    float fireTimer;
    bool hasPowerUp;
    
public:
    Player();
    Player(Vector2 startPos);
    void Update();
    void Draw();
    void Move(Vector2 input);
    void Shoot();
    void UpdateBullets();
    void DrawBullets();
    Rectangle GetRect() const;
    Vector2 GetPosition() const;
    void SetPosition(Vector2 newPos);
    void GivePowerUp();
    bool HasPowerUp() const;
    std::vector<Bullet>& GetBullets();
};

#endif
