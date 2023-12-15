//
// Created by cydeamon on 15.12.2023.
//

#ifndef NOWHERETORUN_MENU_H
#define NOWHERETORUN_MENU_H


#include <list>
#include "../../Engine/Sprite.h"
#include "MenuOption.h"

class Menu
{
public:
    Menu();
    ~Menu();

    void AddOption(MenuOption* option);
    void ShowMenu() { isMenuActive = true; }
    void HideMenu() { isMenuActive = false; }
    void Draw();
    void Update();

    bool IsMenuActive() { return isMenuActive; }


private:
    /***********************************************************/
    /************************* Sprites *************************/

    Sprite* background = nullptr;
    Sprite* logo = nullptr;

    /***********************************************************/
    /********************* Other properties ********************/

    std::list<MenuOption*> options;
    bool isMenuActive = true;
    unsigned int optionHeight = 21;
    unsigned int menuY = 95;
};


#endif //NOWHERETORUN_MENU_H
