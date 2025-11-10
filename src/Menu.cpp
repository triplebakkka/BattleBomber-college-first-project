#include "Menu.h"

Menu::Menu() : selectedOption(0), startGame(false), exitGame(false) {}

void Menu::Update() {
    if (IsKeyPressed(KEY_DOWN)) {
        selectedOption = (selectedOption + 1) % 2;
    } else if (IsKeyPressed(KEY_UP)) {
        selectedOption = (selectedOption - 1 + 2) % 2;
    }
    
    if (IsKeyPressed(KEY_ENTER)) {
        if (selectedOption == 0) {
            startGame = true;
        } else {
            exitGame = true;
        }
    }
}

void Menu::Draw() {
    DrawText("BATTLE BOMBER", 250, 150, 40, WHITE);
    
    // Start option
    if (selectedOption == 0) {
        DrawText("> START GAME", 350, 250, 30, YELLOW);
        DrawText("EXIT", 350, 300, 30, WHITE);
    } else {
        DrawText("START GAME", 350, 250, 30, WHITE);
        DrawText("> EXIT", 350, 300, 30, YELLOW);
    }
}

bool Menu::ShouldStartGame() const {
    return startGame;
}

bool Menu::ShouldExit() const {
    return exitGame;
}

void Menu::Reset() {
    startGame = false;
    exitGame = false;
    selectedOption = 0;
}
