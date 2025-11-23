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
    bool animating;
    float animationTimer;
    float animationOffset;
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
    void UpdateTileAnimations();
    void Draw();
    void DrawDebug(bool debugMode);
    Player& GetPlayer();
    bool AreAllDestructiblesDestroyed();
    bool IsPlayerDead();
    bool IsPlayerOnExit();
    void CheckBulletCollisions();
    bool CheckCollisionWithBarrel(Vector2 position);
    bool CheckCollisionWithObstacles(Vector2 position);
};

#endif
