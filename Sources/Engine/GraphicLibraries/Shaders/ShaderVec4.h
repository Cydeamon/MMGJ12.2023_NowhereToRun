//
// Created by cydeamon on 06.12.2023.
//

#ifndef NOWHERETORUN_SHADERVEC4_H
#define NOWHERETORUN_SHADERVEC4_H

#include <string>

class ShaderVec4
{
public:
    ShaderVec4(float x, float y, float z, float w);
    ShaderVec4();

    void SetX(float x) { this->x = x; validate(); }
    void SetY(float y) { this->y = y; validate(); }
    void SetZ(float z) { this->z = z; validate(); }
    void SetW(float w) { this->w = w; validate(); }

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetZ() const { return z; }
    float GetW() const { return w; }

private:
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

    void validate();
};

#endif //NOWHERETORUN_SHADERVEC4_H
