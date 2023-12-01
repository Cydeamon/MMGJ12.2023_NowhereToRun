//
// Created by cydeamon on 26.11.2023.
//

#ifndef NOWHERETORUN_RENDER_H
#define NOWHERETORUN_RENDER_H

#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Render
{
public:
    static void Init();
    static void InitGLFW();
    static void InitGLAD();
    static unsigned int LoadShader(unsigned int type, std::string filename);
    static void CheckErrors();

    static unsigned int VBO, VAO, EBO;
    static unsigned int ShaderProgram;
};


#endif //NOWHERETORUN_RENDER_H
