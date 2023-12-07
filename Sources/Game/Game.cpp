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
    if (!engine)
        throw std::runtime_error("Engine not initialized");

    // Create vertices
    float vertices[] = {
        // positions         // colors          // texture coords
        0.5f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,  1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,   // bottom left
        -0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, 1.0f    // top left
    };


    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glBindBuffer(GL_ARRAY_BUFFER, OpenGL::API::VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL::API::EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Load texture
    glActiveTexture(GL_TEXTURE0);
    OpenGL::Texture texture("Assets/MessageBG.png");
    glActiveTexture(GL_TEXTURE1);
    OpenGL::Texture texture2("Assets/MessageGo.png");    

    // Load shaders
    OpenGL::API::CreateShaderProgram("Textured");
    OpenGL::API::LoadShader(GL_VERTEX_SHADER, "vert.glsl");
    OpenGL::API::LoadShader(GL_FRAGMENT_SHADER, "frag.glsl");
    OpenGL::API::UseShaderProgram("Textured");

    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("texture1", 0);
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("texture2", 1);

    while (!engine->ShouldClose())
    {
        engine->Update();
        engine->BeginFrameDraw();

        glBindVertexArray(OpenGL::API::VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        OpenGL::API::CheckErrors();
        engine->EndFrameDraw();
    }
}

Game::~Game()
{
    delete engine;
}

