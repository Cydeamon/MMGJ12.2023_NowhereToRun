//
// Created by cydeamon on 26.11.2023.
//

#ifndef NOWHERETORUN_VECTOR2_H
#define NOWHERETORUN_VECTOR2_H

#include <cmath>

namespace Cygine
{
    class Vector2
    {
    public:
        Vector2(double x = 0.0f, double y = 0.0f) : x(x), y(y) {};

        double x = 0;
        double y = 0;
        static const Vector2 Zero;
        static const Vector2 Up;
        static const Vector2 Down;
        static const Vector2 Left;
        static const Vector2 Right;

        /**********************************************************/
        /************************* Methods ************************/

        void Normalize();
        double Length();

        /**********************************************************/
        /************************ Operators ***********************/

        Vector2 operator + (const Vector2 &other) { return Vector2(x + other.x, y + other.y); }
        Vector2 operator - (const Vector2 &other) { return Vector2(x - other.x, y - other.y); }
        Vector2 operator * (const Vector2 &other) { return Vector2(x * other.x, y * other.y); }
        Vector2 operator / (const Vector2 &other) { return Vector2(x / other.x, y / other.y); }

        Vector2 operator + (const float value) { return Vector2(x + value, y + value); }
        Vector2 operator - (const float value) { return Vector2(x - value, y - value); }
        Vector2 operator * (const float value) { return Vector2(x * value, y * value); }
        Vector2 operator / (const float value) { return Vector2(x / value, y / value); }

        bool operator == (const Vector2 &other) { return x == other.x && y == other.y; }
        bool operator != (const Vector2 &other) { return x != other.x || y != other.y; }

        Vector2 &operator += (const Vector2 &other) { x += other.x; y += other.y; return *this; }
        Vector2 &operator -= (const Vector2 &other) { x -= other.x; y -= other.y; return *this; }
        Vector2 &operator *= (const Vector2 &other) { x *= other.x; y *= other.y; return *this; }
        Vector2 &operator /= (const Vector2 &other) { x /= other.x; y /= other.y; return *this; }

        Vector2 &operator += (const float value) { x += value; y += value; return *this; }
        Vector2 &operator -= (const float value) { x -= value; y -= value; return *this; }
        Vector2 &operator *= (const float value) { x *= value; y *= value; return *this; }
        Vector2 &operator /= (const float value) { x /= value; y /= value; return *this; }

    };

} // Cygine

#endif //NOWHERETORUN_VECTOR2_H
