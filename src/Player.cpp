#include "Player.h"
#include "TextureManager.h"

Player::Player() : position{100, 100}, size{30, 30}, color{BLUE}, 
                   speed{3.0f}, direction{0, -1}, fireCooldown(0.5f), 
                   fireTimer(0), hasPowerUp(false) {}

Player::Player(Vector2 startPos) : position{startPos}, size{30, 30}, 
                                   color{BLUE}, speed{3.0f}, direction{0, -1},
                                   fireCooldown(0.5f), fireTimer(0), hasPowerUp(false) {}

void Player::Update() {
    // Update fire timer
    if (fireTimer > 0) {
        fireTimer -= GetFrameTime();
    }
    
    UpdateBullets();
}

void Player::Draw() {
    DrawDebug(false);
}

void Player::DrawDebug(bool debugMode) {
    if (!debugMode) {
        TextureManager* texManager = TextureManager::GetInstance();
        Texture2D tankTexture = texManager->GetTexture("tank");
        
        // Calculate rotation based on direction
        float rotation = 0.0f;
        if (direction.x == 1) rotation = 90.0f;
        else if (direction.x == -1) rotation = 270.0f;
        else if (direction.y == -1) rotation = 0.0f;
        else if (direction.y == 1) rotation = 180.0f;
        
        // Draw tank sprite instead of rectangle
        Rectangle sourceRect = {0, 0, (float)tankTexture.width, (float)tankTexture.height};
        Rectangle destRect = {position.x, position.y, size.x, size.y};
        Vector2 origin = {size.x/2, size.y/2};
        
        DrawTexturePro(tankTexture, sourceRect, destRect, origin, rotation, WHITE);
    } else {
        // Draw player hitbox
        Rectangle playerRect = GetRect();
        DrawRectangleRec(playerRect, BLUE);
        DrawRectangleLinesEx(playerRect, 2.0f, BLACK);
    }
    
    DrawBulletsDebug(debugMode);
}

void Player::Move(Vector2 input) {
    if (input.x != 0 || input.y != 0) {
        direction = input;
    }
    
    Vector2 newPos = {
        position.x + input.x * speed,
        position.y + input.y * speed
    };
    
    position = newPos;
}

void Player::Shoot() {
    if (fireTimer <= 0) {
        bullets.emplace_back(position, direction, hasPowerUp);
        fireTimer = fireCooldown;
    }
}

void Player::UpdateBullets() {
    for (auto it = bullets.begin(); it != bullets.end();) {
        it->Update();
        if (it->ShouldDestroy()) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void Player::DrawBullets() {
    DrawBulletsDebug(false);
}

void Player::DrawBulletsDebug(bool debugMode) {
    for (auto& bullet : bullets) {
        bullet.DrawDebug(debugMode);
    }
}

Rectangle Player::GetRect() const {
    return {position.x - size.x/2, position.y - size.y/2, size.x, size.y};
}

Vector2 Player::GetPosition() const {
    return position;
}

void Player::SetPosition(Vector2 newPos) {
    position = newPos;
}

float Player::GetSpeed() const {
    return speed;
}

void Player::SetDirection(Vector2 dir) {
    if (dir.x != 0 || dir.y != 0) {
        direction = dir;
    }
}

void Player::GivePowerUp() {
    hasPowerUp = true;
    color = PURPLE; // Visual indicator
}

bool Player::HasPowerUp() const {
    return hasPowerUp;
}

std::vector<Bullet>& Player::GetBullets() {
    return bullets;
}
