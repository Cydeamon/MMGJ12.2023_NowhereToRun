//
// Created by cydeamon on 26.11.2023.
//

#include "Engine.h"
#include "GraphicLibraries/OpenGL.h"

Cygine::Engine::Engine()
{
    OpenGL::API::InitGLFW();
    glfwSetErrorCallback(glfwErrorCallback);
    initWindow();
    OpenGL::API::InitGLAD();
    OpenGL::API::Init();
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

    OpenGL::API::CheckErrors();
}

void Cygine::Engine::Update()
{
    updateForcedAspectRatio();
    Input::Update();
    OpenGL::API::CheckErrors();
}

bool Cygine::Engine::ShouldClose()
{
    bool shouldClose = glfwWindowShouldClose(window);
    return shouldClose;
}

void Cygine::Engine::BeginFrameDraw()
{
    lastFrameTime = glfwGetTime();
    OpenGL::API::BeginFrameDraw();
}

void Cygine::Engine::EndFrameDraw()
{
    OpenGL::API::EndFrameDraw();
    delta = glfwGetTime() - lastFrameTime;
    fps = (int) (1.0 / delta);
    OpenGL::API::CheckErrors();
}

void Cygine::Engine::initWindow()
{
    std::string title = std::string(PROJECT_LABEL) + " v" + PROJECT_VER;
    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, title.c_str(), NULL, NULL);

    if (window == NULL)
        throw std::runtime_error("Engine::initWindow(). Failed to create GLFW window");

    glfwMakeContextCurrent(window);
    
    // Make window centered
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(window, (mode->width - DEFAULT_WINDOW_WIDTH) / 2, (mode->height - DEFAULT_WINDOW_HEIGHT) / 2);
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
        throw std::runtime_error("glfwErrorCallback(). " + message);
    else
        std::cerr << "WARNING: " << message << std::endl;
}

double Cygine::Engine::GetDelta() const
{
    return delta;
}

Cygine::Engine::~Engine()
{
    glfwDestroyWindow(window);
    OpenGL::API::Destroy();
}

int Cygine::Engine::GetWindowWidth() const
{
    int width;
    glfwGetWindowSize(window, &width, nullptr);
    return width;
}


int Cygine::Engine::GetWindowHeight() const
{
    int height;
    glfwGetWindowSize(window, nullptr, &height);
    return height;
}

void Cygine::Engine::ForceWindowAspectRatio(float aspect)
{
    forcedAspectRatio = aspect;
}

void Cygine::Engine::updateForcedAspectRatio()
{
    if (forcedAspectRatio != 0 && !IsFullscreen())
    {
        Vector2 resolution = GetWindowResolution();
        float aspect = resolution.x / resolution.y;

        if (forcedAspectRatio - aspect > 0.01 || forcedAspectRatio - aspect < -0.01)
        {
            resolution.x = resolution.y * forcedAspectRatio;
            resolution.y = resolution.x / forcedAspectRatio;
            glfwSetWindowSize(window, (int) resolution.x, (int) resolution.y);
            SetWindowCentered();
        }
    }
}

void Cygine::Engine::ToggleFullscreen()
{
    if (glfwGetWindowMonitor(window) == nullptr)
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(window, nullptr, 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0);
        updateForcedAspectRatio();
        SetWindowCentered();
    }
}

void Cygine::Engine::SetWindowCentered()
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    Vector2 resolution = GetWindowResolution();
    glfwSetWindowPos(window, (mode->width - resolution.x) / 2, (mode->height - resolution.y) / 2);
}

bool Cygine::Engine::IsFullscreen()
{
    return glfwGetWindowMonitor(window) != nullptr;
}

void Cygine::Engine::SetInnerResolution(int x, int y)
{
    OpenGL::API::SetInnerResolution(x, y);
}

