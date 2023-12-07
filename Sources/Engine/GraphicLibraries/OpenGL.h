//
// Created by cydeamon on 26.11.2023.
//

#ifndef NOWHERETORUN_OPENGL_H
#define NOWHERETORUN_OPENGL_H


#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shaders/ShaderProgram.h"
#include "Texture.h"

#include "../../Vendor/stb/stb_image.h"

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

        static unsigned int VBO, VAO, EBO;


    private:
        static std::vector<ShaderProgram *> shaderPrograms;
        static ShaderProgram *currentShaderProgram;
    };
}



#endif //NOWHERETORUN_OPENGL_H
