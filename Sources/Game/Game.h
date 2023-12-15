//
// Created by cydeamon on 28.11.2023.
//

#ifndef NOWHERETORUN_GAME_H
#define NOWHERETORUN_GAME_H

#include "../CMakeConfig.h"
#include "../Engine/Engine.h"
#include "Menu/Menu.h"
#include <fstream>

class Game
{
public:
    Game();
    ~Game();

    void Run();

private:
    Cygine::Engine *engine = nullptr;
    Menu *menu = nullptr;
    MenuOption *startGameOption = nullptr;
    bool isGameWasStarted = false;
    bool isGamePaused = false;

    void initMenu();
    void OnStartGameSelected();
    void OnExitGameSelected();
    void OnHighscoresSelected();
    void OnCreditsSelected();
    void ListenControls();

    /***************************************************************************/
    /********************************* Sprites *********************************/

    Sprite *background = nullptr;

};


#endif //NOWHERETORUN_GAME_H
