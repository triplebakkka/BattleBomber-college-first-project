#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "LevelManager.h"
#include "Menu.h"
#include "TextureManager.h"

enum class GameState {
    MENU,
    PLAYING,
    LEVEL_SELECT,
    GAME_OVER,
    WIN
};

class Game {
private:
    GameState currentState;
    Menu menu;
    LevelManager levelManager;
    Player player;
    int currentLevel;
    float gameTime;
    float levelTimeLimit;
    bool levelCompleted;
    void LoadTextures();

public:
    Game();
    ~Game(); 
    void Run();
    void Update();
    void Draw();
    void StartGame(int level);
    void CheckWinCondition();
    void CheckLoseCondition();
};
 
#endif
