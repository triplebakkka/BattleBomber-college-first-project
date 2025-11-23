#include "Game.h"
#include "raylib.h"

Game::Game() : currentState(GameState::MENU), currentLevel(1), 
               gameTime(0), levelTimeLimit(120.0f), levelCompleted(false), debugMode(false),
               player(nullptr) {
    InitWindow(800, 600, "Battle Bomber");
    SetTargetFPS(60);
    
    // Change to project root directory so asset paths work
    // Try to find project root by looking for src directory
    if (DirectoryExists("src")) {
        // Already in project root
    } else if (DirectoryExists("../src")) {
        ChangeDirectory("..");
    } else if (DirectoryExists("../../src")) {
        ChangeDirectory("../..");
    } else if (DirectoryExists("../../../src")) {
        ChangeDirectory("../../..");
    } else if (DirectoryExists("../../../../src")) {
        ChangeDirectory("../../../..");
    } else if (DirectoryExists("../../../../../src")) {
        ChangeDirectory("../../../../..");
    }
    
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
    
    texManager->LoadTexture("tank", "src/Assets/TankForward.png");
    texManager->LoadTexture("bullet", "src/Assets/bullet.png");
    texManager->LoadTexture("wall", "src/Assets/TileTexture.png");
    texManager->LoadTexture("barrel", "src/Assets/Barrel.png");
    texManager->LoadTexture("powerup", "src/Assets/PowerUp.png");
    texManager->LoadTexture("exit", "src/Assets/ExitSprite.png");
    texManager->LoadTexture("destructible", "src/Assets/DestructibeBlock.png");
}
// Don't forget to clean up in destructor or when closing

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
            
        case GameState::PLAYING: {
            gameTime += GetFrameTime();
            
            // Handle player input - movement (WASD and arrow keys)
            Vector2 input = {0, 0};
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
                input.y = -1;
            }
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
                input.y = 1;
            }
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                input.x = -1;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                input.x = 1;
            }
            
            // Normalize diagonal movement
            if (input.x != 0 && input.y != 0) {
                input.x *= 0.707f; // 1/sqrt(2) for normalized diagonal
                input.y *= 0.707f;
            }
            
            // Check collision before moving - store current position
            // use LevelManager's player via pointer (must be set by StartGame)
            if (!player) break;
            Vector2 currentPos = player->GetPosition();
            Vector2 newPos = {
                currentPos.x + input.x * player->GetSpeed(),
                currentPos.y + input.y * player->GetSpeed()
            };
            
            // Only move if new position doesn't collide with obstacles
            if (!levelManager.CheckCollisionWithObstacles(newPos)) {
                player->Move(input);
            } else {
                // Still update direction even if we can't move
                if (input.x != 0 || input.y != 0) {
                    player->SetDirection(input);
                }
            }
            
            // Handle shooting (spacebar)
            if (IsKeyPressed(KEY_SPACE)) {
                player->Shoot();
            }
            
            // Toggle debug mode with F1
            if (IsKeyPressed(KEY_F1)) {
                debugMode = !debugMode;
            }
            
            // Update level (this updates the LevelManager's player and checks bullets)
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
        }
        
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

    // Set background color based on game state
    Color backgroundColor = WHITE;
    switch (currentState) {
        case GameState::MENU:
            backgroundColor = BLACK;
            break;
        case GameState::LEVEL_SELECT:
        case GameState::PLAYING:
        case GameState::GAME_OVER:
        case GameState::WIN:
            backgroundColor = BLUE;
            break;
    }
    ClearBackground(backgroundColor);
    
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
            levelManager.DrawDebug(debugMode);
            player->DrawDebug(debugMode);

            // Draw HUD
            DrawText(TextFormat("Time: %.1f", levelTimeLimit - gameTime), 10, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", currentLevel), 10, 40, 20, WHITE);
            if (debugMode) {
                DrawText("DEBUG MODE (F1 to toggle)", 10, 70, 20, RED);
            }
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
    // point Game::player to LevelManager's player instance so bullets and input operate on same object
    player = &levelManager.GetPlayer();
    gameTime = 0;
    levelCompleted = false;
    currentState = GameState::PLAYING;
}

void Game::CheckWinCondition() {
    if (!levelCompleted && (levelManager.AreAllDestructiblesDestroyed() || levelManager.IsPlayerOnExit())) {
        levelCompleted = true;
        currentState = GameState::WIN;
    }
}

void Game::CheckLoseCondition() {
    if (levelManager.IsPlayerDead()) {
        currentState = GameState::GAME_OVER;
    }
}
