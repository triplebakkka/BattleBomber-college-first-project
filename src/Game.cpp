#include "Game.h"
#include "raylib.h"

Game::Game() : currentState(GameState::MENU), currentLevel(1), 
               gameTime(0), levelTimeLimit(120.0f), levelCompleted(false) {
    InitWindow(800, 600, "Battle Bomber");
    SetTargetFPS(60);
    
    // Load textures when game starts
    LoadTextures();
}

Game::~Game() {
    // Ensure raylib window closed (safe even if already closed)
    if (!WindowShouldClose()) CloseWindow();
    // Destroy texture manager singleton (its destructor unloads textures)
    TextureManager::DestroyInstance();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        Update();
        Draw();
    }
    CloseWindow();
}

void Game::LoadTextures() {
    TextureManager* texManager = TextureManager::GetInstance();
    
    texManager->LoadTexture("tank", "assets/TankForward.png");
    texManager->LoadTexture("bullet", "assets/bullet.png");
    texManager->LoadTexture("wall", "assets/TileTexture.png");
    texManager->LoadTexture("barrel", "assets/Barrel.png");
    texManager->LoadTexture("powerup", "assets/PowerUp.png");
    texManager->LoadTexture("exit", "assets/ExitSprite.png");
}
// Don't forget to clean up in destructor or when closing

void Game::Run() {
    while (!WindowShouldClose()) {
        Update();
        Draw();
    }
    CloseWindow();
}

void Game::Update() {
    switch (currentState) {
        case GameState::MENU:
            menu.Update();
            if (menu.ShouldStartGame()) {
                currentState = GameState::LEVEL_SELECT;
            }
            if (menu.ShouldExit()) {
                CloseWindow();
            }
            break;
            
        case GameState::LEVEL_SELECT:
            if (IsKeyPressed(KEY_ONE)) {
                StartGame(1);
            } else if (IsKeyPressed(KEY_TWO)) {
                StartGame(2);
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = GameState::MENU;
            }
            break;
            
        case GameState::PLAYING:
            gameTime += GetFrameTime();
            player.Update();
            levelManager.Update();
            
            // Check if time ran out
            if (gameTime >= levelTimeLimit) {
                currentState = GameState::GAME_OVER;
            }
            
            CheckWinCondition();
            CheckLoseCondition();
            
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = GameState::MENU;
            }
            break;
            
        case GameState::GAME_OVER:
        case GameState::WIN:
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = GameState::MENU;
            }
            break;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    switch (currentState) {
        case GameState::MENU:
            menu.Draw();
            break;
            
        case GameState::LEVEL_SELECT:
            DrawText("SELECT LEVEL", 300, 200, 30, WHITE);
            DrawText("1 - Level 1", 350, 250, 20, WHITE);
            DrawText("2 - Level 2", 350, 280, 20, WHITE);
            DrawText("ESC - Back to Menu", 320, 350, 20, WHITE);
            break;
            
        case GameState::PLAYING:
            levelManager.Draw();
            player.Draw();
            
            // Draw HUD
            DrawText(TextFormat("Time: %.1f", levelTimeLimit - gameTime), 10, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", currentLevel), 10, 40, 20, WHITE);
            break;
            
        case GameState::GAME_OVER:
            DrawText("GAME OVER", 300, 250, 40, RED);
            DrawText("Press ENTER to continue", 280, 320, 20, WHITE);
            break;
            
        case GameState::WIN:
            DrawText("LEVEL COMPLETE!", 280, 250, 40, GREEN);
            DrawText("Press ENTER to continue", 280, 320, 20, WHITE);
            break;
    }
    
    EndDrawing();
}

void Game::StartGame(int level) {
    currentLevel = level;
    levelManager.LoadLevel(level);
    player = levelManager.GetPlayer();
    gameTime = 0;
    levelCompleted = false;
    currentState = GameState::PLAYING;
}

void Game::CheckWinCondition() {
    if (!levelCompleted && levelManager.AreAllDestructiblesDestroyed()) {
        levelCompleted = true;
        currentState = GameState::WIN;
    }
}

void Game::CheckLoseCondition() {
    if (levelManager.IsPlayerDead()) {
        currentState = GameState::GAME_OVER;
    }
}
