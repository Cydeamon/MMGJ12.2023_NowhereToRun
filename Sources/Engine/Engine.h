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

        void ForceWindowAspectRatio(float aspect);
        void ToggleFullscreen();
        void SetInnerResolution(int x, int y);
        void SetWindowCentered();
        bool IsFullscreen();

        void SetWindowResolution(int width, int height);
    private:
        double delta = 0;
        int fps = 0;
        double lastFrameTime;
        double forcedAspectRatio = 0;
        bool debug = true;

        void initWindow();
        static void glfwErrorCallback(int error, const char *description);
        void updateForcedAspectRatio();

        Vector2 windowResolution = Vector2(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    };
}

#endif //NOWHERETORUN_ENGINE_H
