//
// Created by cydeamon on 26.11.2023.
//

#ifndef NOWHERETORUN_ENGINE_H
#define NOWHERETORUN_ENGINE_H

#include "GraphicLibraries/OpenGL.h"
#include "../CMakeConfig.h"
#include "Color.h"
#include "Input/Input.h"
#include <iostream>
#include <fstream>

namespace Cygine
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        bool ShouldClose();
        void BeginFrameDraw();
        void EndFrameDraw();
        void Update();

        double GetDelta() const;
        int GetFPS() const { return fps; }
        int GetWindowWidth() const;
        int GetWindowHeight() const;
        float GetWindowAspectRatio() const { return (float) GetWindowWidth() / (float) GetWindowHeight(); }
        Vector2 GetWindowResolution() const { return Vector2(GetWindowWidth(), GetWindowHeight()); }

    private:
        GLFWwindow *window = nullptr;

        void initWindow();
        static void glfwErrorCallback(int error, const char *description);
        static void updateWindowResolutionCallback(GLFWwindow *window, int width, int height);
        double delta = 0;
        int fps = 0;
        double lastFrameTime;
    };
}

#endif //NOWHERETORUN_ENGINE_H
