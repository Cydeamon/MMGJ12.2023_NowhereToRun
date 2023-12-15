//
// Created by cydeamon on 15.12.2023.
//

#include "Menu.h"

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
}

void Menu::Update()
{

}

Menu::~Menu()
{
    delete background;
    delete logo;
}
