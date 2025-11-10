#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "Player.h"
#include "raylib.h"
#include <vector>

enum class TileType {
    EMPTY,
    WALL,
    DESTRUCTIBLE,
    BARREL,
    POWER_UP,
    SPAWN_POINT,
    EXIT_POINT
};

struct Tile {
    TileType type;
    Rectangle rect;
    bool destroyed;
};

class LevelManager {
private:
    std::vector<std::vector<Tile>> currentLevel;
    Player player;
    Vector2 exitPoint;
    int tileSize;
    
public:
    LevelManager();
    void LoadLevel(int levelNumber);
    void Update();
    void Draw();
    Player& GetPlayer();
    bool AreAllDestructiblesDestroyed();
    bool IsPlayerDead();
    void CheckBulletCollisions();
    bool CheckCollisionWithBarrel(Vector2 position);
};

#endif
