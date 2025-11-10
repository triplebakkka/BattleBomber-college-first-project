#ifndef MENU_H
#define MENU_H

#include "raylib.h"

class Menu {
private:
    int selectedOption;
    bool startGame;
    bool exitGame;
    
public:
    Menu();
    void Update();
    void Draw();
    bool ShouldStartGame() const;
    bool ShouldExit() const;
    void Reset();
};

#endif
