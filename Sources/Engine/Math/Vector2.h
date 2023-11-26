//
// Created by cydeamon on 26.11.2023.
//

#ifndef NOWHERETORUN_VECTOR2_H
#define NOWHERETORUN_VECTOR2_H

namespace Cygine
{
    class Vector2
    {
    public:
        Vector2(int x = 0.0f, int y = 0.0f) : x(x), y(y) {};
        Vector2() {};

        int x = 0;
        int y = 0;
    };

} // Cygine

#endif //NOWHERETORUN_VECTOR2_H
