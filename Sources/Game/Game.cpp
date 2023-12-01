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
    // Create vertices
    float vertices[] = {
        0.5f, 0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    glBindBuffer(GL_ARRAY_BUFFER, Render::VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Render::EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Load shaders
    Render::LoadShader(GL_VERTEX_SHADER, "vert.glsl");
    Render::LoadShader(GL_FRAGMENT_SHADER, "frag.glsl");

    while (!engine->ShouldClose())
    {
        engine->Update();
        engine->BeginFrameDraw();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Render::EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        engine->EndFrameDraw();
    }
}

Game::~Game()
{
    delete engine;
}

