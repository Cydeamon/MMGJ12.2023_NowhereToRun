//
// Created by cydeamon on 26.11.2023.
//

#ifndef NOWHERETORUN_OPENGL_H
#define NOWHERETORUN_OPENGL_H


#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shaders/ShaderProgram.h"
#include "Texture.h"

#include "../../Vendor/stb/stb_image.h"
#include "../Sprite.h"

namespace OpenGL
{
    class API
    {
    public:
        static void Init();
        static void Destroy();
        static void InitGLFW();
        static void InitGLAD();
        static void LoadShader(unsigned int type, std::string filename);
        static void CheckErrors();
        static void UseShaderProgram(std::string name);
        static void CreateShaderProgram(std::string name);
        static ShaderProgram* GetCurrentShaderProgram();
        static void DrawSprite(Sprite *sprite);
        static void UpdateInnerResolutionScale(int width, int height);
        static void SetInnerResolution(int x, int y);
        static Cygine::Vector2 GetInnerResolution();
        static void Update();

        static void BeginFrameDraw();
        static void EndFrameDraw();
        static void SetClearColor(Cygine::Color color);
        static double GetTime();
        static void SetWindowResolution(int width, int height);
        static bool IsFullscreen();
        static void SetWindowed();
        static void SetFullscreen();
        static void InitWindow(std::string title, int width, int height);

        static void SetWindowTitle(std::string title);
        static bool IsShouldClose();
        static void SetWindowCentered();
    private:
        static std::vector<ShaderProgram *> shaderPrograms;
        static ShaderProgram *currentShaderProgram;
        static float spriteVertices[];
        static float frameBufferRect[];
        static unsigned int frameBufferVBO, frameBufferVAO, frameBufferRBO;
        static unsigned int spriteVBO, spriteVAO;
        static unsigned int frameBuffer;
        static unsigned int frameTexture;
        static Cygine::Vector2 innerResolutionScale;
        static Cygine::Vector2 innerResolution;
        static GLFWwindow *window;

        static void updateWindowResolutionCallback(GLFWwindow *window, int width, int height);
        static void drawFrameBuffer();
        static void reinitFrameBuffer();
        static Cygine::Vector2 GetWindowResolution();
        static void glfwErrorCallback(int error, const char *description);
    };
}



#endif //NOWHERETORUN_OPENGL_H
