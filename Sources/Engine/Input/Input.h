//
// Created by cydeamon on 27.11.2023.
//

#ifndef NOWHERETORUN_INPUT_H
#define NOWHERETORUN_INPUT_H

#include <map>
#include "../Math/Vector2.h"
#include "InputKeys.h"
#include "BtnState.h"


namespace Cygine
{
    class Input
    {
    public:

        static void Init();
        static void Update();
        static bool IsPressed(KeyboardKey key);
        static bool IsPressed(MouseButton btn);
        static bool IsReleased(MouseButton btn);
        static bool IsReleased(KeyboardKey key);
        static bool IsJustPressed(KeyboardKey key);
        static bool IsJustPressed(MouseButton btn);
        static bool IsJustReleased(Cygine::KeyboardKey key);
        static bool IsJustReleased(Cygine::MouseButton btn);
        static Vector2 GetMousePosition();
        static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void MouseCallback(GLFWwindow *window, int button, int action, int mods);
        static void MouseMoveCallback(GLFWwindow *window, double xpos, double ypos);

    private:
        static std::map<int, BtnState> keyStates;
        static std::map<int, BtnState> mouseStates;
        static Vector2 mousePosition;
    };
}


#endif //NOWHERETORUN_INPUT_H
