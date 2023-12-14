//
// Created by cydeamon on 26.11.2023.
//

#include "Engine.h"
#include "GraphicLibraries/OpenGL.h"
Cygine::Engine::Engine()
{
    std::string title = std::string(PROJECT_LABEL) + " v" + PROJECT_VER;

    OpenGL::API::InitGLFW();
    OpenGL::API::InitWindow(title, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    OpenGL::API::InitGLAD();
    OpenGL::API::Init();
    Input::Init();

    // Init values
    lastFrameTime = OpenGL::API::GetTime();

    // Clear color
    OpenGL::API::SetClearColor(Cygine::Color("#000F1C"));
    OpenGL::API::CheckErrors();
}

void Cygine::Engine::Update()
{
    updateForcedAspectRatio();
    Input::Update();
    OpenGL::API::CheckErrors();

    if (debug)
    {
        OpenGL::API::SetWindowTitle(std::string(PROJECT_LABEL) + " v" + PROJECT_VER + " - FPS: " + std::to_string(fps));
    }
}

bool Cygine::Engine::ShouldClose()
{
    return OpenGL::API::IsShouldClose();
}

void Cygine::Engine::BeginFrameDraw()
{
    lastFrameTime = OpenGL::API::GetTime();
    OpenGL::API::BeginFrameDraw();
}

void Cygine::Engine::EndFrameDraw()
{
    OpenGL::API::EndFrameDraw();
    delta = glfwGetTime() - lastFrameTime;
    fps = (int) (1.0 / delta);
    OpenGL::API::CheckErrors();
}

double Cygine::Engine::GetDelta() const
{
    return delta;
}

Cygine::Engine::~Engine()
{
    OpenGL::API::Destroy();
}

int Cygine::Engine::GetWindowWidth() const
{
    return windowResolution.x;
}

int Cygine::Engine::GetWindowHeight() const
{
    return windowResolution.y;
}

void Cygine::Engine::ForceWindowAspectRatio(float aspect)
{
    forcedAspectRatio = aspect;
}

void Cygine::Engine::updateForcedAspectRatio()
{
    if (forcedAspectRatio != 0 && !IsFullscreen())
    {
        float aspect = windowResolution.x / windowResolution.y;

        if (forcedAspectRatio - aspect > 0.01 || forcedAspectRatio - aspect < -0.01)
        {
            windowResolution.x *= forcedAspectRatio;
            windowResolution.y /= forcedAspectRatio;
            OpenGL::API::SetWindowResolution(windowResolution.x, windowResolution.y);
            SetWindowCentered();
        }
    }
}

void Cygine::Engine::ToggleFullscreen()
{
    if (OpenGL::API::IsFullscreen())
    {
        OpenGL::API::SetWindowed();
        OpenGL::API::SetWindowResolution(windowResolution.x, windowResolution.y);
        SetWindowCentered();
    }
    else
    {
        OpenGL::API::SetFullscreen();
    }
}

void Cygine::Engine::SetWindowCentered()
{
    OpenGL::API::SetWindowCentered();
}

bool Cygine::Engine::IsFullscreen()
{
    return OpenGL::API::IsFullscreen();
}

void Cygine::Engine::SetInnerResolution(int x, int y)
{
    OpenGL::API::SetInnerResolution(x, y);
}

void Cygine::Engine::SetWindowResolution(int width, int height)
{
    windowResolution = Vector2(width, height);
    OpenGL::API::SetWindowResolution(width, height);
    SetWindowCentered();
}

