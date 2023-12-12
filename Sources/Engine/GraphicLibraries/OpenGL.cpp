//
// Created by cydeamon on 26.11.2023.
//

#include "OpenGL.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

using namespace OpenGL;

unsigned int API::spriteVBO, API::spriteVAO;
ShaderProgram *API::currentShaderProgram = nullptr;
std::vector<ShaderProgram *> API::shaderPrograms;
Cygine::Vector2 API::innerResolutionScale;
Cygine::Vector2 API::innerResolution;
bool API::usingCustomInnerResolution = false;
unsigned int API::frameBuffer;
unsigned int API::frameTexture;

float API::spriteVertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

void API::Init()
{
    int width, height;
    if (glfwGetWindowMonitor(glfwGetCurrentContext()) != nullptr)
            glfwGetMonitorPos(glfwGetWindowMonitor(glfwGetCurrentContext()), &width, &height);
    else
        glfwGetWindowPos(glfwGetCurrentContext(), &width, &height);


    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(glfwGetCurrentContext(), updateWindowResolutionCallback);

    // Setup sprite buffers
    glGenVertexArrays(1, &spriteVAO);
    glGenBuffers(1, &spriteVBO);
    glBindVertexArray(spriteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spriteVertices), spriteVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    CheckErrors();

    // Init shaders
    CreateShaderProgram("Sprites");
    LoadShader(GL_VERTEX_SHADER, "Sprite.vert");
    LoadShader(GL_FRAGMENT_SHADER, "Sprite.frag");


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
        : shaderPrograms[shaderPrograms.size() - 1];

    // Check if shader type is valid
    if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER && type != GL_COMPUTE_SHADER && type != GL_GEOMETRY_SHADER)
        throw std::runtime_error("GraphicLibraries::LoadShader(). Invalid shader type: " + std::to_string(type));

    // Check if file exists
    if (!std::ifstream("Shaders/" + filename))
        throw std::runtime_error("GraphicLibraries::LoadShader(). Shader file not found: Shaders/" + filename);

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
            case GL_VERTEX_SHADER: shaderType = "vertex";
                break;
            case GL_FRAGMENT_SHADER: shaderType = "fragment";
                break;
            case GL_COMPUTE_SHADER: shaderType = "compute";
                break;
            case GL_GEOMETRY_SHADER: shaderType = "geometry";
                break;
        }

        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        throw std::runtime_error("Shader (type: " + shaderType + ", file: " + filename + ") compilation failed\n" + infoLog);
    }

    // Apply shader to current shader program
    switch (type)
    {
        case GL_VERTEX_SHADER: shaderProgram->SetVertexShader(shader);
            break;
        case GL_FRAGMENT_SHADER: shaderProgram->SetFragmentShader(shader);
            break;

        default:throw std::runtime_error("GraphicLibraries::LoadShader(). This shader type currently not supported: " + std::to_string(type));
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
            case GL_INVALID_ENUM:errorDescription = "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
                break;
            case GL_INVALID_VALUE:errorDescription = "GL_INVALID_VALUE: A numeric argument is out of range.";
                break;
            case GL_INVALID_OPERATION:errorDescription = "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
                break;
            case GL_OUT_OF_MEMORY:errorDescription = "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:errorDescription = "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
                break;
            case GL_CONTEXT_LOST:errorDescription = "GL_CONTEXT_LOST: The API context was lost.";
                break;
            case GL_TABLE_TOO_LARGE:errorDescription = "GL_TABLE_TOO_LARGE: The specified table exceeds the implementation's maximum supported table size.";
                break;
            case GL_INVALID_INDEX:errorDescription = "GL_INVALID_INDEX: An invalid index was specified.";
                break;
            default:errorDescription = "Unknown API error.";
                break;
        }
    }

    if (!errorDescription.empty())
        throw std::runtime_error("GraphicLibraries::CheckErrors(). GraphicLibraries error: " + errorDescription);

    // Check framebuffer errors
    int framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (framebufferStatus)
        {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: The framebuffer is not complete.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: No image is attached to the framebuffer.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: The frame buffer attached images have mismatching dimensions.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: The frame buffer attached images have mismatching image formats.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: The frame buffer attached images have mismatching draw buffers.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: The frame buffer attached images have mismatching read buffers.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: The frame buffer attached images have mismatching multisample.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: The frame buffer attached images have mismatching layer targets.";
                break;
            default:
                errorDescription = "Unknown framebuffer error.";
        }

        throw std::runtime_error("GraphicLibraries::CheckErrors(). Framebuffer error: " + errorDescription);
    }
}

void API::UseShaderProgram(std::string name)
{
    // If current shader program is the same as the requested one - do nothing
    if (currentShaderProgram != nullptr && currentShaderProgram->GetName() == name)
        return;

    // Otherwise - Find shader program
    bool shaderProgramFound = false;

    for (ShaderProgram *shaderProgram: shaderPrograms)
    {
        if (shaderProgram->GetName() == name)
        {
            shaderProgramFound = true;
            shaderProgram->Use();
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

ShaderProgram *API::GetCurrentShaderProgram()
{
    if (currentShaderProgram == nullptr)
        throw std::runtime_error("GraphicLibraries::GetCurrentShaderProgram(). Current shader program not set.");

    return currentShaderProgram;
}

void API::Destroy()
{
    for (ShaderProgram *shaderProgram: shaderPrograms)
    {
        delete shaderProgram;
    }

    glDeleteBuffers(1, &spriteVAO);
    glDeleteBuffers(1, &spriteVBO);
    glfwTerminate();
}

void API::DrawSprite(Sprite *sprite)
{
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(sprite->GetPosition().x, sprite->GetPosition().y, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
    model = glm::translate(model, glm::vec3(0.5f * sprite->GetSize().x, 0.5f * sprite->GetSize().y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(sprite->GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * sprite->GetSize().x, -1.5 * sprite->GetSize().y, 0.0f)); // move origin back
    model = glm::scale(model, glm::vec3(sprite->GetSize().x, sprite->GetSize().y, 1.0f)); // last scale

    glm::mat4 projection = glm::ortho(0.0f, (float) width, (float) -height, 0.0f, -1.0f, 1.0f);

    OpenGL::API::UseShaderProgram("Sprites");
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("model", model);
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("spriteColor", sprite->GetColor().GetShaderNormalized());
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("image", 0);
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("projection", projection);

    glBindVertexArray(spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void API::UpdateInnerResolutionScale()
{
    int width, height;

    if (glfwGetWindowMonitor(glfwGetCurrentContext()) != nullptr)
        glfwGetMonitorPos(glfwGetWindowMonitor(glfwGetCurrentContext()), &width, &height);
    else
        glfwGetWindowPos(glfwGetCurrentContext(), &width, &height);

    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    innerResolutionScale.x = (float) innerResolution.x / (float) width;
    innerResolutionScale.y = (float) innerResolution.y / (float) height;

    glViewport(0, 0, width, height);
}

void API::SetInnerResolution(int x, int y)
{
    innerResolution.x = x;
    innerResolution.y = y;
    UpdateInnerResolutionScale();
    usingCustomInnerResolution = true;
}

void API::BeginFrameDraw()
{
    // glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    CheckErrors();
}

void API::EndFrameDraw()
{
    // drawFrameBuffer();
}

void API::updateWindowResolutionCallback(GLFWwindow *window, int width, int height)
{
    // Update inner resolution if it wasn't set
    if (!usingCustomInnerResolution)
    {
        if (glfwGetWindowMonitor(glfwGetCurrentContext()) != nullptr)
            glfwGetMonitorPos(glfwGetWindowMonitor(glfwGetCurrentContext()), &width, &height);
        else
            glfwGetWindowPos(glfwGetCurrentContext(), &width, &height);

        innerResolution.x = width;
        innerResolution.y = height;
    }

    glViewport(0, 0, width, height);
}

void OpenGL::API::drawFrameBuffer()
{



}
