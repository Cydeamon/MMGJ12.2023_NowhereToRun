//
// Created by cydeamon on 26.11.2023.
//

#ifndef NOWHERETORUN_COLOR_H
#define NOWHERETORUN_COLOR_H

#include <string>

namespace Cygine
{
    class Color
    {
    public:
        Color (const std::string& hex);

        float GetNormalizedR() { return r / 255.0f; }
        float GetNormalizedG() { return g / 255.0f; }
        float GetNormalizedB() { return b / 255.0f; }
        float GetNormalizedA() { return a / 255.0f; }
        int GetR() { return r; }
        int GetG() { return g; }
        int GetB() { return b; }
        int GetA() { return a; }
        void SetHex(const std::string &hex);

    private:
        int r = 0;
        int g = 0;
        int b = 0;
        int a = 255;

    };

} // Cygine

#endif //NOWHERETORUN_COLOR_H
