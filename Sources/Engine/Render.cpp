//
// Created by cydeamon on 26.11.2023.
//

#include "Render.h"

unsigned int Render::VBO, Render::VAO, Render::EBO;
unsigned int Render::ShaderProgram;

void Render::Init()
{
    // Create and setup buffers
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    Render::CheckErrors();

    // Bind attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    Render::CheckErrors();

    // Create shader program
    ShaderProgram = glCreateProgram();

    Render::CheckErrors();
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
    // Create and compile shader
    unsigned int shader = glCreateShader(type);
    std::ifstream shaderFile("Shaders/" + filename);
    std::string shaderString((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
    const char *shaderSource = shaderString.c_str();
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check for compilation errors
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

    // Attach shader
    glAttachShader(ShaderProgram, shader);
    glLinkProgram(ShaderProgram);
    glUseProgram(ShaderProgram);

    // Check for linking errors
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
        throw "Shader program linking failed\n" + std::string(infoLog);
    }

    // Cleanup and check for errors
    glDeleteShader(shader);
    Render::CheckErrors();

    // Return shader id
    return shader;
}

void Render::CheckErrors()
{
    std::string errorDescription;
    GLenum error;

    while ((error = glGetError()) != GL_NO_ERROR)
    {
        switch (error)
        {
            case GL_INVALID_ENUM:
                errorDescription = "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
                break;
            case GL_INVALID_VALUE:
                errorDescription = "GL_INVALID_VALUE: A numeric argument is out of range.";
                break;
            case GL_INVALID_OPERATION:
                errorDescription = "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
                break;
            case GL_OUT_OF_MEMORY:
                errorDescription = "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorDescription = "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
                break;
            case GL_CONTEXT_LOST:
                errorDescription = "GL_CONTEXT_LOST: The OpenGL context was lost.";
                break;
            case GL_TABLE_TOO_LARGE:
                errorDescription = "GL_TABLE_TOO_LARGE: The specified table exceeds the implementation's maximum supported table size.";
                break;
            case GL_INVALID_INDEX:
                errorDescription = "GL_INVALID_INDEX: An invalid index was specified.";
                break;
            default:
                errorDescription = "Unknown OpenGL error.";
                break;
        }
    }

    if (!errorDescription.empty())
        throw std::runtime_error("Render error: " + errorDescription);
}

