//
// Created by cydeamon on 28.11.2023.
//

#include "Game.h"
#include "../Engine/Sprite.h"
#include "../Engine/Shapes.h"
#include "Player.h"

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

    background = new Sprite("Assets/Level/LevelBG.png");
}

void Game::initMenu()
{
    menu = new Menu();
    startGameOption = menu->AddOption(new MenuOption("Assets/Menu/OptionStartGame.png", "Assets/Menu/OptionStartGameSelected.png", std::bind(&Game::OnStartGameSelected, this)));
    menu->AddOption(new MenuOption("Assets/Menu/OptionHighscores.png", "Assets/Menu/OptionHighscoresSelected.png", std::bind(&Game::OnHighscoresSelected, this)));
    menu->AddOption(new MenuOption("Assets/Menu/OptionCredits.png", "Assets/Menu/OptionCreditsSelected.png", std::bind(&Game::OnCreditsSelected, this)));
    menu->AddOption(new MenuOption("Assets/Menu/OptionExit.png", "Assets/Menu/OptionExitSelected.png", std::bind(&Game::OnExitGameSelected, this)));
}

void Game::OnStartGameSelected()
{
    std::cout << "Start game / Continue" << std::endl;

    if (!isGameWasStarted)
    {
        player = new Player();

        // Replace "StartGame" with "Continue" texture
        startGameOption->ReplaceStandartSprite("Assets/Menu/OptionContinue.png");
        startGameOption->ReplaceSelectedSprite("Assets/Menu/OptionContinueSelected.png");
    }

    // Start game
    isGamePaused = false;
    isGameWasStarted = true;
    menu->HideMenu();
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
    Line line(5, 5, DEFAULT_WINDOW_WIDTH - 5, DEFAULT_WINDOW_HEIGHT - 5);
    line.color = Cygine::Color::Green();

    Circle circle(0, 0, 20);
    circle.color = Cygine::Color::Blue();

    Rectangle rectangle(50, 50, 10, 10);
    rectangle.color = Cygine::Color::Red();

    while (!engine->ShouldClose())
    {
        /*********************************************/
        /******************** Updates ****************/

        ListenControls();
        menu->Update();

        if (isGameWasStarted && !isGamePaused)
        {
            if (player)
                player->Update();
        }

        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::ARROW_LEFT))
        {
            line.end.x -= 1;
        }
        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::ARROW_RIGHT))
        {
            line.end.x += 1;
        }
        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::ARROW_UP))
        {
            line.end.y -= 1;
        }
        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::ARROW_DOWN))
        {
            line.end.y += 1;
        }


        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::A))
        {
            line.position.x -= 1;
        }
        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::D))
        {
            line.position.x += 1;
        }
        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::W))
        {
            line.position.y -= 1;
        }
        if (Cygine::Input::IsPressed(Cygine::KeyboardKey::S))
        {
            line.position.y += 1;
        }


        /*********************************************/
        /******************** Draws *****************/

        engine->Update();
        engine->BeginFrameDraw();

        if (!isGameWasStarted || isGamePaused)
            menu->Draw();
        else
        {
            background->Draw();
            player->Draw();
        }

        std::cout << line.position.x << " " << line.position.y << std::endl;
        line.Draw();
        circle.Draw();
        rectangle.Draw();

        engine->EndFrameDraw();
    }
}

void Game::ListenControls()
{
    // Fullscreen toggle
    if (Cygine::Input::IsJustPressed(Cygine::KeyboardKey::F11))
        engine->ToggleFullscreen();

    // Pause
    if (Cygine::Input::IsJustPressed(Cygine::KeyboardKey::ESCAPE))
    {
        if (isGameWasStarted)
        {
            isGamePaused = !isGamePaused;

            if (isGamePaused)
                menu->ShowMenu();
            else
                menu->HideMenu();
        }
    }
}

Game::~Game()
{
    delete menu;
    delete engine;
    delete background;
}


