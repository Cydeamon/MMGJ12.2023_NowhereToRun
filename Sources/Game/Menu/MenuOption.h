//
// Created by cydeamon on 15.12.2023.
//

#ifndef NOWHERETORUN_MENUOPTION_H
#define NOWHERETORUN_MENUOPTION_H


#include <functional>
#include "../../Engine/Sprite.h"
#include "../../Engine/GraphicLibraries/OpenGL.h"


class MenuOption
{
public:
    MenuOption(std::string standartSpritePath, std::string selectedSpritePath, std::function<void()> callback);
    ~MenuOption();

    void Update();
    void Draw();
    void SetPositionY(int y);

    void SetSelected(bool selected) { isSelected = selected; }
    void SetCallback(void (*callback)()) { this->callback = callback; }

    void ReplaceStandartSprite(std::string string);
    void ReplaceSelectedSprite(std::string string);
private:
    Sprite *selectedSprite = nullptr;
    Sprite *standartSprite = nullptr;
    bool isSelected = false;
    std::function<void()> callback = nullptr;
};


#endif //NOWHERETORUN_MENUOPTION_H
