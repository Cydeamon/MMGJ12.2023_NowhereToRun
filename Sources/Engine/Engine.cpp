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
    Render::Init();

    glfwSetErrorCallback(glfwErrorCallback);
    Input::Init();

    // Init values
    lastFrameTime = glfwGetTime();

    // Clear color
    Cygine::Color color("#000F1C");

    glClearColor(
        color.GetNormalizedR(),
        color.GetNormalizedG(),
        color.GetNormalizedB(),
        color.GetNormalizedA()
    );

    Render::CheckErrors();
}

void Cygine::Engine::Update()
{
    Input::Update();
    Render::CheckErrors();
}

bool Cygine::Engine::ShouldClose()
{
    bool shouldClose = glfwWindowShouldClose(window);
    return shouldClose;
}

void Cygine::Engine::BeginFrameDraw()
{
    lastFrameTime = glfwGetTime();
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(Render::VAO);
}

void Cygine::Engine::EndFrameDraw()
{
    glfwSwapBuffers(window);
    glfwPollEvents();
    delta = glfwGetTime() - lastFrameTime;
}

void Cygine::Engine::initWindow()
{
    std::string title = std::string(PROJECT_LABEL) + " v" + PROJECT_VER;
    window = glfwCreateWindow(960, 540, title.c_str(), NULL, NULL);

    if (window == NULL)
        throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);

    // Resize callback
    glfwSetFramebufferSizeCallback(window, updateWindowResolutionCallback);
}

void Cygine::Engine::glfwErrorCallback(int error, const char *description)
{
    if (error == GLFW_NO_ERROR)
        return;

    bool fatal = true;
    std::string message = std::string("GLFW error (") + std::to_string(error) + "): " + description + "\n";

    if (error == GLFW_INVALID_ENUM)
        message += "Invalid enum\n";

    if (error == GLFW_INVALID_VALUE)
        message += "Invalid value\n";

    if (error == GLFW_OUT_OF_MEMORY)
        message += "Out of memory\n";

    if (fatal)
        throw std::runtime_error(message);
    else
        std::cerr << "WARNING: " << message << std::endl;
}

void Cygine::Engine::updateWindowResolutionCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

double Cygine::Engine::GetDelta() const
{
    return delta;
}

Cygine::Engine::~Engine()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

