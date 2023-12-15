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
    menu = new Menu();
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

