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
}

void Game::Run()
{
    if (!engine)
        throw std::runtime_error("Engine not initialized");

    engine->SetInnerResolution(384, 216);
    engine->SetWindowResolution(1280, 720);
    engine->ForceWindowAspectRatio(16.0 / 9.0);

    // Load shaders
    Sprite sprite("Assets/Menu.png");

    while (!engine->ShouldClose())
    {
        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::ARROW_RIGHT))
            sprite.Move(1.0f, 0.0f);

        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::ARROW_LEFT))
            sprite.Move(-1.0f, 0.0f);

        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::ARROW_UP))
            sprite.Move(0.0f, 1.0f);

        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::ARROW_DOWN))
            sprite.Move(0.0f, -1.0f);

        if (Cygine::Input::IsJustPressed(Cygine::KeyboardKey::F11))
            engine->ToggleFullscreen();

        engine->Update();
        engine->BeginFrameDraw();

        sprite.Draw();

        engine->EndFrameDraw();
    }
}

Game::~Game()
{
    delete engine;
}

