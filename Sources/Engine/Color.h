//
// Created by cydeamon on 26.11.2023.
//

#ifndef NOWHERETORUN_COLOR_H
#define NOWHERETORUN_COLOR_H

#include <string>
#include "GraphicLibraries/Shaders/ShaderVec4.h"

namespace Cygine
{
    class Color
    {
    public:
        Color (const std::string& hex);

        ShaderVec4 GetShaderNormalized() { return ShaderVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); }
        float GetNormalizedR() { return r / 255.0f; }
        float GetNormalizedG() { return g / 255.0f; }
        float GetNormalizedB() { return b / 255.0f; }
        float GetNormalizedA() { return a / 255.0f; }
        int GetR() { return r; }
        int GetG() { return g; }
        int GetB() { return b; }
        int GetA() { return a; }

        void SetR(int r);
        void SetG(int g);
        void SetB(int b);
        void SetAlpha(int a);

        void SetHex(const std::string &hex);

        // Default colors
        static Color Red() { return Color("#FF0000"); }
        static Color Green() { return Color("#00FF00"); }
        static Color Blue() { return Color("#0000FF"); }
        static Color Black() { return Color("#000000"); }
        static Color White() { return Color("#FFFFFF"); }
        static Color Yellow() { return Color("#FFFF00"); }
        static Color Cyan() { return Color("#00FFFF"); }
        static Color Magenta() { return Color("#FF00FF"); }

    private:
        int r = 0;
        int g = 0;
        int b = 0;
        int a = 255;

    };

} // Cygine

#endif //NOWHERETORUN_COLOR_H
