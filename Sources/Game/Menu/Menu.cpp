//
// Created by cydeamon on 15.12.2023.
//

#include "Menu.h"
#include "../../Engine/GraphicLibraries/OpenGL.h"
Menu::Menu()
{
    background = new Sprite("Assets/Menu/Background.png");
    background->SetPosition(0, 0);
    logo = new Sprite("Assets/Menu/Title.png");
    logo->SetPosition(Sprite::Alignment::CENTER, 13);
}

void Menu::Draw()
{
    background->Draw();
    logo->Draw();

    for (MenuOption* option : options)
        option->Draw();
}

void Menu::Update()
{
    if (isMenuActive)
    {
        for (MenuOption* option : options)
            option->Update();
    }
}

Menu::~Menu()
{
    delete background;
    delete logo;
}

void Menu::AddOption(MenuOption *option)
{
    int optionY = menuY + (optionHeight * options.size());
    options.push_back(option);
    option->SetPositionY(optionY);
}
