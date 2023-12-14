//
// Created by cydeamon on 26.11.2023.
//

#include "OpenGL.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

using namespace OpenGL;

unsigned int API::spriteVBO = 0, API::spriteVAO = 0;
unsigned int API::frameBufferVBO = 0, API::frameBufferVAO = 0, API::frameBufferRBO = 0;
ShaderProgram *API::currentShaderProgram = nullptr;
GLFWwindow *API::window = nullptr;

std::vector<ShaderProgram *> API::shaderPrograms;
Cygine::Vector2 API::innerResolutionScale;
Cygine::Vector2 API::innerResolution;
unsigned int API::frameBuffer = 0;
unsigned int API::frameTexture = 0;

float API::spriteVertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

float API::frameBufferRect[] = {
    // pos      // tex
    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,

    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f
};

void API::Init()
{
    glfwSwapInterval(0);
    glfwSetErrorCallback(glfwErrorCallback);
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

    CreateShaderProgram("FrameBuffer");
    LoadShader(GL_VERTEX_SHADER, "FrameBuffer.vert");
    LoadShader(GL_FRAGMENT_SHADER, "FrameBuffer.frag");

    // Init frame buffer
    Cygine::Vector2 resolution = GetWindowResolution();

    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &frameTexture);
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0);

    glGenVertexArrays(1, &frameBufferVAO);
    glGenBuffers(1, &frameBufferVBO);
    glBindVertexArray(frameBufferVAO);
    glBindBuffer(GL_ARRAY_BUFFER, frameBufferVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frameBufferRect), frameBufferRect, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    glGenRenderbuffers(1, &frameBufferRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, frameBufferRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.x, resolution.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frameBufferRBO);

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
            case GL_INVALID_FRAMEBUFFER_OPERATION:errorDescription = "GL_INVALID_FRAMEBUFFER_OPERATION: The frameBuffer object is not complete.";
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

    // Check frameBuffer errors
    int frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (frameBufferStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (frameBufferStatus)
        {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: The frameBuffer is not complete.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: No image is attached to the frameBuffer.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: The frame buffer attached images have mismatching dimensions.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: The frame buffer attached images have mismatching image formats.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: The frame buffer attached images have mismatching draw buffers.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: The frame buffer attached images have mismatching read buffers.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: The frame buffer attached images have mismatching multisample.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:errorDescription = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: The frame buffer attached images have mismatching layer targets.";
                break;
            default:errorDescription = "Unknown frameBuffer error.";
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
    glfwTerminate();

    for (ShaderProgram *shaderProgram: shaderPrograms)
        delete shaderProgram;

    glDeleteBuffers(1, &spriteVAO);
    glDeleteBuffers(1, &spriteVBO);
    glfwDestroyWindow(window);
}

void API::DrawSprite(Sprite *sprite)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(sprite->GetPosition().x, sprite->GetPosition().y - sprite->GetSize().y, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * sprite->GetPosition().x, 0.5f * sprite->GetPosition().y, 0.0f));
    model = glm::rotate(model, glm::radians(sprite->GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * sprite->GetPosition().x, -0.5f * sprite->GetPosition().y, 0.0f));
    model = glm::scale(model, glm::vec3(sprite->GetSize().x, sprite->GetSize().y, 1.0f));

    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<float>(GetInnerResolution().x),
        -static_cast<float>(GetInnerResolution().y),
        0.0f,
        -1.0f,
        1.0f
    );

    OpenGL::API::UseShaderProgram("Sprites");
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("model", model);
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("spriteColor", sprite->GetColor().GetShaderNormalized());
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("image", 1);
    OpenGL::API::GetCurrentShaderProgram()->SetUniformValue("projection", projection);

    glBindVertexArray(spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

Cygine::Vector2 API::GetInnerResolution()
{
    Cygine::Vector2 resolution = GetWindowResolution();

    return Cygine::Vector2(
        innerResolution.x == 0 ? resolution.x : innerResolution.x,
        innerResolution.y == 0 ? resolution.y : innerResolution.y
    );
}

void API::SetInnerResolution(int x, int y)
{
    innerResolution.x = x;
    innerResolution.y = y;
}

void API::BeginFrameDraw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, GetInnerResolution().x, GetInnerResolution().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    CheckErrors();
}

void API::EndFrameDraw()
{
    Cygine::Vector2 resolution = GetWindowResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    UseShaderProgram("FrameBuffer");
    glBindVertexArray(frameBufferVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(glfwGetCurrentContext());
    glfwPollEvents();
    CheckErrors();
}

void API::updateWindowResolutionCallback(GLFWwindow *window, int width, int height)
{

}

Cygine::Vector2 API::GetWindowResolution()
{
    int width, height;

    if (glfwGetWindowMonitor(glfwGetCurrentContext()) != nullptr)
        glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
    else
        glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

    return Cygine::Vector2(width, height);
}


void API::glfwErrorCallback(int error, const char *description)
{
    if (error == GLFW_NO_ERROR)
        return;

    bool fatal = true;
    std::string message = std::string("GLFW error (") + std::to_string(error) + "): " + description + "\n";

    if (error == GLFW_INVALID_ENUM)
        message += "Invalid enum\n";

    if (error == GLFW_INVALID_VALUE)
        message += "Invalid value\n";

    if (error == GLFW_OUT_OF_MEMORY)
        message += "Out of memory\n";

    if (fatal)
        throw std::runtime_error("glfwErrorCallback(). " + message);
    else
        std::cerr << "WARNING: " << message << std::endl;
}

void API::SetClearColor(Cygine::Color color)
{
    glClearColor(
        color.GetNormalizedR(),
        color.GetNormalizedG(),
        color.GetNormalizedB(),
        color.GetNormalizedA()
    );
}

double API::GetTime()
{
    return glfwGetTime();
}

void API::SetWindowResolution(int width, int height)
{
    glfwSetWindowSize(glfwGetCurrentContext(), width, height);
}

bool API::IsFullscreen()
{
    return glfwGetWindowMonitor(glfwGetCurrentContext()) != nullptr;
}

void API::SetWindowed()
{
    glfwSetWindowMonitor(glfwGetCurrentContext(), nullptr, 0, 0, GetWindowResolution().x, GetWindowResolution().y, GLFW_DONT_CARE);
}

void API::SetFullscreen()
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(glfwGetCurrentContext(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void API::InitWindow(std::string title, int width, int height)
{
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (window == NULL)
        throw std::runtime_error("Engine::initWindow(). Failed to create GLFW window");

    glfwMakeContextCurrent(window);

    // Make window centered
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
}

void API::SetWindowTitle(std::string title)
{
    glfwSetWindowTitle(window, title.c_str());
}

bool API::IsShouldClose()
{
    return glfwWindowShouldClose(window);
}

void API::SetWindowCentered()
{
    if (!IsFullscreen())
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        glfwSetWindowPos(window, (mode->width - GetWindowResolution().x) / 2, (mode->height - GetWindowResolution().y) / 2);
    }
}
