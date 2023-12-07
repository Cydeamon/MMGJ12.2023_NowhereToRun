//
// Created by cydeamon on 07.12.2023.
//

#include "ShaderVec4.h"

ShaderVec4::ShaderVec4()
{

}

ShaderVec4::ShaderVec4(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    validate();
}

void ShaderVec4::validate()
{
    if (x > 1.0f || x < 0.0f || y > 1.0f || y < 0.0f || z > 1.0f || z < 0.0f || w > 1.0f || w < 0.0f)
    {
        throw "ShaderVec4 value out of range: " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + " " + std::to_string(w);
    }
}
