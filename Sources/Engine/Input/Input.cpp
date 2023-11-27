//
// Created by cydeamon on 27.11.2023.
//

#include <iostream>
#include "Input.h"

namespace Cygine
{

    std::map<int, BtnState> Input::keyStates;
    std::map<int, BtnState> Input::mouseStates;
    Cygine::Vector2 Input::mousePosition;

    void Input::Init()
    {
        glfwSetKeyCallback(glfwGetCurrentContext(), Input::KeyCallback);
        glfwSetCursorPosCallback(glfwGetCurrentContext(), Input::MouseMoveCallback);
        glfwSetMouseButtonCallback(glfwGetCurrentContext(), Input::MouseCallback);
    }

    void Input::Update()
    {
        // Update states
        for (auto &key : keyStates)
            key.second.ResetJustTriggered();

        for (auto &mouse : mouseStates)
            mouse.second.ResetJustTriggered();
    }

    bool Input::IsJustPressed(KeyboardKey key)
    {
        if (keyStates.find(key) == keyStates.end())
            return false;

        return keyStates[key].state == InputState::JUST_PRESSED;
    }

    bool Input::IsJustPressed(MouseButton btn)
    {
        if (mouseStates.find(btn) == mouseStates.end())
            return false;

        return mouseStates[btn].state == InputState::JUST_PRESSED;
    }

    bool Input::IsJustReleased(KeyboardKey key)
    {
        if (keyStates.find(key) == keyStates.end())
            return false;

        return keyStates[key].state == InputState::JUST_RELEASED;
    }

    bool Input::IsJustReleased(MouseButton btn)
    {
        if (mouseStates.find(btn) == mouseStates.end())
            return false;

        return mouseStates[btn].state == InputState::JUST_RELEASED;
    }

    bool Input::IsPressed(Cygine::MouseButton btn)
    {
        if (mouseStates.find(btn) == mouseStates.end())
            return false;

        return mouseStates[btn].state == InputState::PRESSED;
    }

    bool Input::IsPressed(Cygine::KeyboardKey key)
    {
        if (keyStates.find(key) == keyStates.end())
            return false;

        return keyStates[key].state == InputState::PRESSED;
    }

    bool Input::IsReleased(Cygine::MouseButton btn)
    {
        if (mouseStates.find(btn) == mouseStates.end())
            return false;

        return mouseStates[btn].state == InputState::RELEASED;
    }

    bool Input::IsReleased(Cygine::KeyboardKey key)
    {
        if (keyStates.find(key) == keyStates.end())
            return false;

        return keyStates[key].state == InputState::RELEASED;
    }

    Cygine::Vector2 Input::GetMousePosition()
    {
        return mousePosition;
    }

    void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_UNKNOWN)
            return;

        // Create state for button if not exist
        if (keyStates.find(key) == keyStates.end())
            keyStates[key] = BtnState();

        // Update state
        if (action == GLFW_PRESS)
            keyStates[key].SetPressed();
        else if (action == GLFW_RELEASE)
            keyStates[key].SetReleased();
    }

    void Input::MouseCallback(GLFWwindow* window, int button, int action, int mods)
    {
        // Create state for button if not exist
        if (mouseStates.find(button) == mouseStates.end())
            mouseStates[button] = BtnState();

        // Update state
        if (action == GLFW_PRESS)
            mouseStates[button].SetPressed();
        else if (action == GLFW_RELEASE)
            mouseStates[button].SetReleased();
    }

    void Input::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
    {
        mousePosition.x = xpos;
        mousePosition.y = ypos;
    }
}




