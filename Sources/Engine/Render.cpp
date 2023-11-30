//
// Created by cydeamon on 26.11.2023.
//

#include "Render.h"

unsigned int Render::VBO, Render::VAO;

void Render::Init()
{
    // Create and setup buffers
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Bind attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Render::InitGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Render::InitGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");
}

unsigned int Render::LoadShader(unsigned int type, std::string filename)
{
    unsigned int shader = glCreateShader(type);
    std::ifstream shaderFile("Shaders/" + filename);
    std::string shaderString((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
    const char *shaderSource = shaderString.c_str();
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        std::string shaderType;

        switch (type)
        {
            case GL_VERTEX_SHADER: shaderType = "vertex"; break;
            case GL_FRAGMENT_SHADER: shaderType = "fragment"; break;
            case GL_COMPUTE_SHADER: shaderType = "compute"; break;
            case GL_GEOMETRY_SHADER: shaderType = "geometry"; break;
        }

        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        throw "Shader (type: " + shaderType + ", file: " + filename + ") compilation failed\n" + infoLog;
    }

    return shader;
}

