//
// Created by cydeamon on 26.11.2023.
//

#include "Engine.h"
#include "Render.h"

Cygine::Engine::Engine()
{
    Render::InitGLFW();
    initWindow();
    Render::InitGLAD();
    glfwSetErrorCallback(glfwErrorCallback);

    // Clear color
    Cygine::Color color("#000F1C");

    glClearColor(
        color.GetNormalizedR(),
        color.GetNormalizedG(),
        color.GetNormalizedB(),
        color.GetNormalizedA()
    );

}

void Cygine::Engine::initWindow()
{
    std::string title = std::string(PROJECT_LABEL) + " v" + PROJECT_VER;
    window = glfwCreateWindow(960, 540, title.c_str(), NULL, NULL);

    if (window == NULL)
        glfwTerminate();

    glfwMakeContextCurrent(window);

    // Resize callback
    glfwSetFramebufferSizeCallback(window, updateWindowResolutionCallback);
}

void Cygine::Engine::glfwErrorCallback(int error, const char *description)
{
    throw std::runtime_error(std::string("GLFW error (") + std::to_string(error) + "): " + description);
}

bool Cygine::Engine::ShouldClose()
{
    bool shouldClose = glfwWindowShouldClose(window);
    return shouldClose;
}

Cygine::Engine::~Engine()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Cygine::Engine::updateWindowResolutionCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Cygine::Engine::Update()
{
    glfwPollEvents();
}

void Cygine::Engine::BeginFrameDraw()
{
    glClear(GL_COLOR_BUFFER_BIT);
};

void Cygine::Engine::EndFrameDraw()
{
    glfwSwapBuffers(window);
};
