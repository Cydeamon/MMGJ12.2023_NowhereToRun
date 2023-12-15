//
// Created by cydeamon on 15.12.2023.
//

#include "MenuOption.h"
#include "../../Engine/Input/Input.h"

MenuOption::MenuOption(std::string standartSpritePath, std::string selectedSpritePath, std::function<void()> callback)
{
    this->callback = callback;

    selectedSprite = new Sprite(selectedSpritePath);
    selectedSprite->SetPosition(Sprite::Alignment::CENTER, 0);

    standartSprite = new Sprite(standartSpritePath);
    standartSprite->SetPosition(Sprite::Alignment::CENTER, 0);
}

void MenuOption::Update()
{
    // Check if hovered
    Cygine::Vector2 scale(
        OpenGL::API::GetWindowResolution().x / OpenGL::API::GetInnerResolution().x,
        OpenGL::API::GetWindowResolution().y / OpenGL::API::GetInnerResolution().y
    );

    Cygine::Vector2 mousePosition = Cygine::Input::GetMousePosition();
    mousePosition.x = (int)(mousePosition.x / scale.x);
    mousePosition.y = (int)(mousePosition.y / scale.y);

    isSelected =
        mousePosition.x > standartSprite->GetPosition().x &&
        mousePosition.x < standartSprite->GetPosition().x + standartSprite->GetSize().x &&
        mousePosition.y > standartSprite->GetPosition().y &&
        mousePosition.y < standartSprite->GetPosition().y + standartSprite->GetSize().y;

    // Check if clicked
    if (isSelected && Cygine::Input::IsJustPressed(Cygine::MouseButton::LEFT))
        if (callback)
            callback();
}

void MenuOption::Draw()
{
    if (isSelected)
        selectedSprite->Draw();
    else
        standartSprite->Draw();
}

MenuOption::~MenuOption()
{
    delete selectedSprite;
    delete standartSprite;
}

void MenuOption::SetPositionY(int y)
{
    standartSprite->SetPosition(Sprite::Alignment::CENTER, y);
    selectedSprite->SetPosition(Sprite::Alignment::CENTER, y);
}

void MenuOption::ReplaceStandartSprite(std::string string)
{
    Sprite *newSprite = new Sprite(string);
    newSprite->SetPosition(Sprite::Alignment::CENTER, standartSprite->GetPosition().y);
    delete standartSprite;
    standartSprite = newSprite;
}

void MenuOption::ReplaceSelectedSprite(std::string string)
{
    Sprite *newSprite = new Sprite(string);
    newSprite->SetPosition(Sprite::Alignment::CENTER, selectedSprite->GetPosition().y);
    delete selectedSprite;
    selectedSprite = newSprite;
}

