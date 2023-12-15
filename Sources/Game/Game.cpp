//
// Created by cydeamon on 28.11.2023.
//

#include "Game.h"
#include "../Engine/Sprite.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Game::Game()
{
    engine = new Cygine::Engine();

    if (!engine)
        throw std::runtime_error("Engine not initialized");

    engine->SetInnerResolution(384, 216);
    engine->SetWindowResolution(1280, 720);
    engine->ForceWindowAspectRatio(16.0 / 9.0);

    initMenu();
}

void Game::initMenu()
{
    menu = new Menu();
    menu->AddOption(new MenuOption("Assets/Menu/OptionStartGame.png", "Assets/Menu/OptionStartGameSelected.png", std::bind(&Game::OnStartGameSelected, this)));
    menu->AddOption(new MenuOption("Assets/Menu/OptionHighscores.png", "Assets/Menu/OptionHighscoresSelected.png", std::bind(&Game::OnHighscoresSelected, this)));
    menu->AddOption(new MenuOption("Assets/Menu/OptionCredits.png", "Assets/Menu/OptionCreditsSelected.png", std::bind(&Game::OnCreditsSelected, this)));
    menu->AddOption(new MenuOption("Assets/Menu/OptionExit.png", "Assets/Menu/OptionExitSelected.png", std::bind(&Game::OnExitGameSelected, this)));
}

void Game::OnStartGameSelected()
{
    std::cout << "Start game" << std::endl;
}

void Game::OnCreditsSelected()
{
    std::cout << "Credits" << std::endl;
}

void Game::OnHighscoresSelected()
{
    std::cout << "Highscores" << std::endl;
}

void Game::OnExitGameSelected()
{
    std::cout << "Exit game" << std::endl;
    engine->SetShouldClose();
}

void Game::Run()
{
    while (!engine->ShouldClose())
    {
        /*********************************************/
        /******************** Updates ****************/

        menu->Update();

        /*********************************************/
        /******************** Draws *****************/

        if (Cygine::Input::IsJustPressed(Cygine::KeyboardKey::F11))
            engine->ToggleFullscreen();

        engine->Update();
        engine->BeginFrameDraw();

        menu->Draw();

        engine->EndFrameDraw();
    }
}

Game::~Game()
{
    delete menu;
    delete engine;
}

