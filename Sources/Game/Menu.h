//
// Created by cydeamon on 15.12.2023.
//

#ifndef NOWHERETORUN_MENU_H
#define NOWHERETORUN_MENU_H


#include "../Engine/Sprite.h"

class Menu
{
public:
    Menu();
    ~Menu();

    void Draw();
    void Update();

private:
    /***********************************************************/
    /************************* Sprites *************************/

    Sprite* background = nullptr;
    Sprite* logo = nullptr;
};


#endif //NOWHERETORUN_MENU_H
