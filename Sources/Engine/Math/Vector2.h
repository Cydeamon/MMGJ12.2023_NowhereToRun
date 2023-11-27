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
        Vector2(double x = 0.0f, double y = 0.0f) : x(x), y(y) {};

        double x = 0;
        double y = 0;
    private:
    };

} // Cygine

#endif //NOWHERETORUN_VECTOR2_H
