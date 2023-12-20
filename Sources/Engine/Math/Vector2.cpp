//
// Created by cydeamon on 26.11.2023.
//

#include "Vector2.h"

namespace Cygine
{
    const Vector2 Vector2::Zero(0.0f, 0.0f);
    const Vector2 Vector2::Up(0.0f, 1.0f);
    const Vector2 Vector2::Down(0.0f, -1.0f);
    const Vector2 Vector2::Left(-1.0f, 0.0f);
    const Vector2 Vector2::Right(1.0f, 0.0f);

    void Vector2::Normalize()
    {
        double length = Length();

        if (length != 0)
        {
            x /= length;
            y /= length;
        }
    }

    double Vector2::Length()
    {
        return x * x + y * y != 0 ? sqrt(x * x + y * y) : 0;
    }
}