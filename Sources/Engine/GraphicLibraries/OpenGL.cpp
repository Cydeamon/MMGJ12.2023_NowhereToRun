//
// Created by cydeamon on 26.11.2023.
//

#include "OpenGL.h"
using namespace OpenGL;

unsigned int API::VBO, API::VAO, API::EBO;
ShaderProgram *API::currentShaderProgram = nullptr;
std::vector<ShaderProgram *> API::shaderPrograms;

void API::Init()
{
    // Create and setup buffers
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    CheckErrors();

    // Bind attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    CheckErrors();

    // Unbind
    glBindVertexArray(0);

    CheckErrors();
}

void API::InitGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void API::InitGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("GraphicLibraries::InitGLAD(). Failed to initialize GLAD");

    CheckErrors();
}

/**
 * Loads shader from file. Expects filename from "Shaders" folder.
 * If no shader in use - will use last one or throw error if there is no shader in use
 *
 * @param type     Shader type from opengl
 * @param filename Shader filename
 */
void API::LoadShader(unsigned int type, std::string filename)
{
    // Check if shader programs is created
    if (shaderPrograms.size() == 0)
        throw std::runtime_error("GraphicLibraries::LoadShader(). No shader programs is created");

    ShaderProgram *shaderProgram = 
        currentShaderProgram 
            ? currentShaderProgram
            : shaderPrograms[shaderPrograms.size()-1];

    // Check if shader type is valid
    if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER && type != GL_COMPUTE_SHADER && type != GL_GEOMETRY_SHADER)
        throw std::runtime_error("GraphicLibraries::LoadShader(). Invalid shader type: " + std::to_string(type));

    // Check if file exists
    if (!std::ifstream("Shaders/" + filename))
        throw std::runtime_error("GraphicLibraries::LoadShader(). Shader file not found: " + filename);

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
        throw std::runtime_error("Shader (type: " + shaderType + ", file: " + filename + ") compilation failed\n" + infoLog);
    }

    // Apply shader to current shader program
    switch (type)
    {
        case GL_VERTEX_SHADER: shaderProgram->SetVertexShader(shader); break;
        case GL_FRAGMENT_SHADER: shaderProgram->SetFragmentShader(shader); break;

        default:
            throw std::runtime_error("GraphicLibraries::LoadShader(). This shader type currently not supported: " + std::to_string(type));
    }

    // Cleanup and check for errors
    glDeleteShader(shader);
    CheckErrors();
}

void API::CheckErrors()
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
                errorDescription = "GL_CONTEXT_LOST: The API context was lost.";
                break;
            case GL_TABLE_TOO_LARGE:
                errorDescription = "GL_TABLE_TOO_LARGE: The specified table exceeds the implementation's maximum supported table size.";
                break;
            case GL_INVALID_INDEX:
                errorDescription = "GL_INVALID_INDEX: An invalid index was specified.";
                break;
            default:
                errorDescription = "Unknown API error.";
                break;
        }
    }

    if (!errorDescription.empty())
        throw std::runtime_error("GraphicLibraries::CheckErrors(). GraphicLibraries error: " + errorDescription);
}

void API::UseShaderProgram(std::string name)
{
    bool shaderProgramFound = false;

    for (ShaderProgram* shaderProgram : shaderPrograms)
    {
        if (shaderProgram->GetName() == name)
        {
            shaderProgramFound = true;
            shaderProgram->Use();
            CheckErrors();
            currentShaderProgram = shaderProgram;
            break;
        }
    }

    if (!shaderProgramFound)
        throw std::runtime_error("GraphicLibraries::UseShaderProgram(). Shader program not found: " + name);
}

void API::CreateShaderProgram(std::string name)
{
    shaderPrograms.push_back(new ShaderProgram(name));
}

ShaderProgram* API::GetCurrentShaderProgram()
{
    if (currentShaderProgram == nullptr)
        throw std::runtime_error("GraphicLibraries::GetCurrentShaderProgram(). Current shader program not set.");

    return currentShaderProgram;
}

void API::Destroy()
{
    for (ShaderProgram* shaderProgram : shaderPrograms)
        delete shaderProgram;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
}

