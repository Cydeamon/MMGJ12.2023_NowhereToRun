//
// Created by cydeamon on 27.11.2023.
//

#ifndef NOWHERETORUN_BTNSTATE_H
#define NOWHERETORUN_BTNSTATE_H

#include <GLFW/glfw3.h>
#include "Input.h"

namespace Cygine
{
    enum InputState
    {
        JUST_PRESSED,
        JUST_RELEASED,
        PRESSED,
        RELEASED
    };

    struct BtnState
    {
        InputState state;
        double pressTime;
        double releaseTime;

        void ResetJustTriggered()
        {
            if (state == InputState::JUST_PRESSED)
                state = InputState::PRESSED;

            if (state == InputState::JUST_RELEASED)
                state = InputState::RELEASED;
        }

        void SetPressed()
        {
            state = InputState::JUST_PRESSED;
            pressTime = glfwGetTime();
        }

        void SetReleased()
        {
            state = InputState::JUST_RELEASED;
            releaseTime = glfwGetTime();
        }
    };

}
#endif //NOWHERETORUN_BTNSTATE_H
