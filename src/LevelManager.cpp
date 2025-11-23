#include "LevelManager.h"
#include "TextureManager.h"
#include <iostream>
#include <cmath>

LevelManager::LevelManager() : tileSize(40) {}

void LevelManager::LoadLevel(int levelNumber) {
    currentLevel.clear();
    
    // Simple level design - 20x15 grid
    int width = 20;
    int height = 15;
    
    currentLevel.resize(height, std::vector<Tile>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Tile tile;
            tile.rect = {x * tileSize * 1.0f, y * tileSize * 1.0f, tileSize * 1.0f, tileSize * 1.0f};
            tile.destroyed = false;
            tile.animating = false;
            tile.animationTimer = 0.0f;
            tile.animationOffset = 0.0f;
            
            // Border walls
            if (x == 0 || y == 0 || x == width - 1 || y == height - 1) {
                tile.type = TileType::WALL;
            }
            // Some destructible blocks
            else if ((x + y) % 4 == 0 && levelNumber == 1) {
                tile.type = TileType::DESTRUCTIBLE;
            }
            else if ((x * y) % 5 == 0 && levelNumber == 2) {
                tile.type = TileType::DESTRUCTIBLE;
            }
            // Barrels
            else if ((x + y) % 7 == 0) {
                tile.type = TileType::BARREL;
            }
            // Power-up
            else if (x == 5 && y == 5 && levelNumber == 1) {
                tile.type = TileType::POWER_UP;
            }
            else if (x == 15 && y == 10 && levelNumber == 2) {
                tile.type = TileType::POWER_UP;
            }
            else {
                tile.type = TileType::EMPTY;
            }
            
            // Set spawn and exit points
            if (x == 1 && y == 1) {
                tile.type = TileType::SPAWN_POINT;
                player = Player({tile.rect.x + tileSize/2, tile.rect.y + tileSize/2});
            }
            if (x == width - 2 && y == height - 2) {
                tile.type = TileType::EXIT_POINT;
                exitPoint = {tile.rect.x + tileSize/2, tile.rect.y + tileSize/2};
            }
            
            currentLevel[y][x] = tile;
        }
    }
}

void LevelManager::Update() {
    player.Update();
    CheckBulletCollisions();

    // Update tile animations
    UpdateTileAnimations();

    // Check if player reached exit (alternative win condition)
    Vector2 playerPos = player.GetPosition();
    if (CheckCollisionPointRec(playerPos, {exitPoint.x - 20, exitPoint.y - 20, 40, 40})) {
        // Player reached exit - you could implement this as another win condition
    }
}

void LevelManager::UpdateTileAnimations() {
    for (auto& row : currentLevel) {
        for (auto& tile : row) {
            if (tile.animating) {
                tile.animationTimer -= GetFrameTime();

                // Create jitter effect on x-axis
                tile.animationOffset = std::sin(tile.animationTimer * 50.0f) * 3.0f;

                // End animation and destroy tile
                if (tile.animationTimer <= 0.0f) {
                    tile.destroyed = true;
                    tile.animating = false;
                    tile.animationOffset = 0.0f;
                }
            }
        }
    }
}

void LevelManager::Draw() {
    DrawDebug(false);
}

void LevelManager::DrawDebug(bool debugMode) {
    TextureManager* texManager = TextureManager::GetInstance();
    
    for (const auto& row : currentLevel) {
        for (const auto& tile : row) {
            if (tile.destroyed) continue;
            
            if (debugMode) {
                // Draw hitboxes instead of sprites
                Color hitboxColor = RED;
                switch (tile.type) {
                    case TileType::WALL:
                        hitboxColor = GRAY;
                        break;
                    case TileType::BARREL:
                        hitboxColor = RED;
                        break;
                    case TileType::DESTRUCTIBLE:
                        hitboxColor = ORANGE;
                        break;
                    case TileType::POWER_UP:
                        hitboxColor = YELLOW;
                        break;
                    case TileType::EXIT_POINT:
                        hitboxColor = GREEN;
                        break;
                    default:
                        hitboxColor = DARKGRAY;
                        break;
                }
                Rectangle debugRect = tile.rect;
                debugRect.x += tile.animationOffset; // Apply animation offset
                DrawRectangleRec(debugRect, hitboxColor);
                DrawRectangleLinesEx(debugRect, 2.0f, BLACK);
            } else {
                // Draw sprites normally
                const char* textureName = nullptr;
                switch (tile.type) {
                    case TileType::WALL:
                        textureName = "wall";
                        break;
                    case TileType::DESTRUCTIBLE:
                        textureName = "destructible";
                        break;
                    case TileType::BARREL:
                        textureName = "barrel";
                        break;
                    case TileType::POWER_UP:
                        textureName = "powerup";
                        break;
                    case TileType::EXIT_POINT:
                        textureName = "exit";
                        break;
                    default:
                        break;
                }
                
                if (textureName) {
                    Texture2D texture = texManager->GetTexture(textureName);
                Rectangle drawRect = tile.rect;
                drawRect.x += tile.animationOffset; // Apply animation offset

                DrawTexturePro(texture,
                    {0, 0, (float)texture.width, (float)texture.height},
                    drawRect,
                    {0, 0}, 0, WHITE);
                }
            }
        }
    }
}

Player& LevelManager::GetPlayer() {
    return player;
}

bool LevelManager::AreAllDestructiblesDestroyed() {
    for (const auto& row : currentLevel) {
        for (const auto& tile : row) {
            if (tile.type == TileType::DESTRUCTIBLE && !tile.destroyed) {
                return false;
            }
        }
    }
    return true;
}

bool LevelManager::IsPlayerDead() {
    return CheckCollisionWithBarrel(player.GetPosition());
}

bool LevelManager::IsPlayerOnExit() {
    Vector2 playerPos = player.GetPosition();
    Rectangle playerRect = {playerPos.x - 15, playerPos.y - 15, 30, 30};

    // Check if player rectangle overlaps with exit point area
    Rectangle exitRect = {exitPoint.x - 20, exitPoint.y - 20, 40, 40};
    return CheckCollisionRecs(playerRect, exitRect);
}

void LevelManager::CheckBulletCollisions() {
    auto& bullets = player.GetBullets();
    
    for (auto& bullet : bullets) {
        Rectangle bulletHitbox = bullet.GetHitbox();
        
        for (int y = 0; y < currentLevel.size(); y++) {
            for (int x = 0; x < currentLevel[y].size(); x++) {
                auto& tile = currentLevel[y][x];
                
                if (tile.destroyed) continue;
                
                // Use rectangle-rectangle collision for more accurate hitbox detection
                if (CheckCollisionRecs(bulletHitbox, tile.rect)) {
                    if (tile.type == TileType::DESTRUCTIBLE ||
                        tile.type == TileType::BARREL ||
                        tile.type == TileType::POWER_UP) {

                        if (tile.type == TileType::POWER_UP) {
                            player.GivePowerUp();
                        }

                        // If bullet has power up, destroy adjacent tiles
                        if (bullet.HasPowerUp()) {
                            for (int dy = -1; dy <= 1; dy++) {
                                for (int dx = -1; dx <= 1; dx++) {
                                    int nx = x + dx;
                                    int ny = y + dy;
                                    if (nx >= 0 && nx < currentLevel[y].size() &&
                                        ny >= 0 && ny < currentLevel.size()) {
                                        auto& adjacentTile = currentLevel[ny][nx];
                                        if (adjacentTile.type == TileType::DESTRUCTIBLE ||
                                            adjacentTile.type == TileType::BARREL) {
                                            // Start animation for adjacent tiles too
                                            if (!adjacentTile.animating && !adjacentTile.destroyed) {
                                                adjacentTile.animating = true;
                                                adjacentTile.animationTimer = 0.3f; // 0.3 seconds animation
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // Start destruction animation instead of immediate destruction
                        if (!tile.animating && !tile.destroyed) {
                            tile.animating = true;
                            tile.animationTimer = 0.3f; // 0.3 seconds animation
                        }

                        bullet.MarkForDestruction();
                        break;
                    } else if (tile.type == TileType::WALL) {
                        bullet.MarkForDestruction();
                        break;
                    }
                }
            }
        }
    }
}

bool LevelManager::CheckCollisionWithBarrel(Vector2 position) {
    Rectangle playerRect = {position.x - 15, position.y - 15, 30, 30};
    
    for (const auto& row : currentLevel) {
        for (const auto& tile : row) {
            if (!tile.destroyed && tile.type == TileType::BARREL && 
                CheckCollisionRecs(playerRect, tile.rect)) {
                return true;
            }
        }
    }
    return false;
}

bool LevelManager::CheckCollisionWithObstacles(Vector2 position) {
    Rectangle playerRect = {position.x - 15, position.y - 15, 30, 30};
    
    for (const auto& row : currentLevel) {
        for (const auto& tile : row) {
            if (tile.destroyed) continue;
            
            // Check collision with barrels, walls, and destructible blocks
            if ((tile.type == TileType::BARREL || tile.type == TileType::WALL || tile.type == TileType::DESTRUCTIBLE) &&
                CheckCollisionRecs(playerRect, tile.rect)) {
                return true;
            }
        }
    }
    return false;
}

