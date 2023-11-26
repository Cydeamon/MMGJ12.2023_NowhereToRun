#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "CMakeConfig.h"
#include "Engine/Render.h"
#include "Engine/Engine.h"

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    const char* title = std::string(std::string(PROJECT_LABEL) + " v" + PROJECT_VER).c_str();
    GLFWwindow* window = glfwCreateWindow(960, 540, title, NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Check GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Resize callback
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    // Clear color
    Cygine::Color color("#000F1C");

    glClearColor(
        color.GetNormalizedR(),
        color.GetNormalizedG(),
        color.GetNormalizedB(),
        color.GetNormalizedA()
    );

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll events
        glfwPollEvents();
        // Clear window
        glClear(GL_COLOR_BUFFER_BIT);
        // Swap buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
