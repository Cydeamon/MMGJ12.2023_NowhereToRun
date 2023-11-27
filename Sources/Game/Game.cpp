//
// Created by cydeamon on 28.11.2023.
//

#include "Game.h"

Game::Game()
{
    engine = new Cygine::Engine();
}

void Game::Run()
{
    while (!engine->ShouldClose())
    {
        engine->Update();
        engine->BeginFrameDraw();

        // TODO: Render stuff


        engine->EndFrameDraw();
    }
}

Game::~Game()
{
    delete engine;
}

