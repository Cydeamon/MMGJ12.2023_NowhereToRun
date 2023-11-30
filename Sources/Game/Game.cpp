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
        -0.5, -0.5, 0.0,
        0.5, -0.5, 0.0,
        0.0, 0.5, 0.0
    };

    glBindBuffer(GL_ARRAY_BUFFER, Render::VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Load shaders
    unsigned int vertexShader = Render::LoadShader(GL_VERTEX_SHADER, "vert.glsl");
    unsigned int fragmentShader = Render::LoadShader(GL_FRAGMENT_SHADER, "frag.glsl");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        throw "Shader program linking failed\n" + std::string(infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!engine->ShouldClose())
    {
        engine->Update();
        engine->BeginFrameDraw();

        glUseProgram(shaderProgram);
        glBindVertexArray(Render::VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        engine->EndFrameDraw();
    }
}

Game::~Game()
{
    delete engine;
}

