//
// Created by cydeamon on 06.12.2023.
//

#include "ShaderProgram.h"
#include "../OpenGL.h"
#include "glm/gtc/type_ptr.hpp"

using namespace OpenGL;

ShaderProgram::ShaderProgram(std::string name)
{
    this->name = name;
    id = glCreateProgram();
    API::CheckErrors();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(id);
}

void ShaderProgram::Use()
{
    glUseProgram(id);
    API::CheckErrors();
}

void ShaderProgram::SetVertexShader(int shaderID)
{
    vertexShader = shaderID;
    glAttachShader(id, vertexShader);
    glLinkProgram(id);
    checkLinkingErrors();
    API::CheckErrors();
    updateUniforms();
}

void ShaderProgram::SetFragmentShader(int shaderID)
{
    fragmentShader = shaderID;
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    checkLinkingErrors();
    API::CheckErrors();
    updateUniforms();
}

void ShaderProgram::checkLinkingErrors()
{
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        throw "Shader program linking failed\n" + std::string(infoLog);
    }
}

void ShaderProgram::updateUniforms()
{
    GLint numActiveUniforms;
    char uniformName[256];
    GLsizei length;
    GLint size;
    GLenum type;

    uniforms.clear();
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

    for (int i = 0; i < numActiveUniforms; ++i)
    {
        glGetActiveUniform(id, i, sizeof(uniformName), &length, &size, &type, uniformName);
        uniforms[uniformName] = glGetUniformLocation(id, uniformName);
    }
}

void ShaderProgram::SetUniformValue(std::string name, ShaderVec4 value)
{
    // Check errors
    if (uniforms.find(name) == uniforms.end())
        return;

    int expectedType = GL_FLOAT_VEC4;
    if (getUniformType(name) != expectedType)
    {
        throw std::runtime_error(
            "ShaderProgram::SetUniformValue(). Invalid uniform type: " +
            name +
            ". Expected: " +
            getTypeName(expectedType) +
            ", but got: " +
            getTypeName(getUniformType(name)));
    }

    glUniform4f(uniforms[name], value.GetX(), value.GetY(), value.GetZ(), value.GetW());
}

void ShaderProgram::SetUniformValue(std::string name, float value)
{
    // Check errors
    if (uniforms.find(name) == uniforms.end())
        return;

    int expectedType = GL_FLOAT;
    if (getUniformType(name) != expectedType)
    {
        throw std::runtime_error(
            "ShaderProgram::SetUniformValue(). Invalid uniform type: " +
            name +
            ". Expected: " +
            getTypeName(expectedType) +
            ", but got: " +
            getTypeName(getUniformType(name)));
    }

    // Set value
    glUniform1f(uniforms[name], value);
}

void OpenGL::ShaderProgram::SetUniformValue(std::string name, int value)
{
    // Check errors
    if (uniforms.find(name) == uniforms.end())
        return;

    int expectedTypes[] = {GL_INT, GL_SAMPLER_2D};
    bool found = false;

    for (auto type: expectedTypes)
    {
        if (getUniformType(name) == type)
        {
            glUniform1i(uniforms[name], value);
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::string expectedTypes;

        for (int i = 0; i < sizeof(expectedTypes) / sizeof(expectedTypes[0]); ++i)
        {
            expectedTypes += getTypeName(expectedTypes[i]);

            if (i != sizeof(expectedTypes) / sizeof(expectedTypes[0]) - 1)
                expectedTypes += ", ";
        }

        throw std::runtime_error(
            "ShaderProgram::SetUniformValue(). Invalid uniform type: " + name + ". Expected: " + expectedTypes + ", but got: " + getTypeName(getUniformType(name))
        );
    }

    // Set value
    glUniform1i(uniforms[name], value);
}

void ShaderProgram::SetUniformValue(std::string name, glm::mat4 value)
{
    // Check errors
    if (uniforms.find(name) == uniforms.end())
        return;

    int expectedType = GL_FLOAT_MAT4;
    if (getUniformType(name) != expectedType)
    {
        throw std::runtime_error(
            "ShaderProgram::SetUniformValue(). Invalid uniform type: " +
            name +
            ". Expected: " +
            getTypeName(expectedType) +
            ", but got: " +
            getTypeName(getUniformType(name)));
    }


    glUniformMatrix4fv(uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::SetUniformValue(std::string name, glm::vec2 value)
{
    // Check errors
    if (uniforms.find(name) == uniforms.end())
        throw std::runtime_error("ShaderProgram::SetUniformValue(). Invalid uniform name: " + name);

    int expectedType = GL_FLOAT_VEC2;
    if (getUniformType(name) != expectedType)
    {
        throw std::runtime_error(
            "ShaderProgram::SetUniformValue(). Invalid uniform type: " +
            name +
            ". Expected: " +
            getTypeName(expectedType) +
            ", but got: " +
            getTypeName(getUniformType(name))
        );
    }

    glUniform2f(uniforms[name], value.x, value.y);
}

void ShaderProgram::SetUniformValue(std::string name, Cygine::Vector2 value)
{
    SetUniformValue(name, glm::vec2(value.x, value.y));
}

GLenum ShaderProgram::getUniformType(const std::string &name)
{
    GLenum uniformType;
    GLint size;
    glGetActiveUniform(id, uniforms[name], NULL, NULL, &size, &uniformType, NULL);
    return uniformType;
}

std::string ShaderProgram::getTypeName(GLenum type)
{
    switch (type)
    {
        case GL_FLOAT: return "GL_FLOAT";
        case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
        case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
        case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
        case GL_INT: return "GL_INT";
        case GL_INT_VEC2: return "GL_INT_VEC2";
        case GL_INT_VEC3: return "GL_INT_VEC3";
        case GL_INT_VEC4: return "GL_INT_VEC4";
        case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
        case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
        case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
        case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
        case GL_BOOL: return "GL_BOOL";
        case GL_BOOL_VEC2: return "GL_BOOL_VEC2";
        case GL_BOOL_VEC3: return "GL_BOOL_VEC3";
        case GL_BOOL_VEC4: return "GL_BOOL_VEC4";
        case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
        case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
        case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
        case GL_SAMPLER_1D: return "GL_SAMPLER_1D";
        case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
        case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
        case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE";
        case GL_SAMPLER_1D_SHADOW: return "GL_SAMPLER_1D_SHADOW";
        case GL_SAMPLER_2D_SHADOW: return "GL_SAMPLER_2D_SHADOW";
        case GL_SAMPLER_2D_RECT: return "GL_SAMPLER_2D_RECT";
        case GL_SAMPLER_2D_RECT_SHADOW: return "GL_SAMPLER_2D_RECT_SHADOW";
        case GL_SAMPLER_1D_ARRAY: return "GL_SAMPLER_1D_ARRAY";
        case GL_SAMPLER_2D_ARRAY: return "GL_SAMPLER_2D_ARRAY";
        case GL_SAMPLER_1D_ARRAY_SHADOW: return "GL_SAMPLER_1D_ARRAY_SHADOW";
        case GL_SAMPLER_2D_ARRAY_SHADOW: return "GL_SAMPLER_2D_ARRAY_SHADOW";
        case GL_SAMPLER_CUBE_SHADOW: return "GL_SAMPLER_CUBE_SHADOW";
        case GL_SAMPLER_BUFFER: return "GL_SAMPLER_BUFFER";
        case GL_SAMPLER_2D_MULTISAMPLE: return "GL_SAMPLER_2D_MULTISAMPLE";
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_SAMPLER_CUBE_MAP_ARRAY: return "GL_SAMPLER_CUBE_MAP_ARRAY";
        case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW: return "GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW";
        case GL_INT_SAMPLER_1D: return "GL_INT_SAMPLER_1D";
        case GL_INT_SAMPLER_2D: return "GL_INT_SAMPLER_2D";
        case GL_INT_SAMPLER_3D: return "GL_INT_SAMPLER_3D";
        case GL_INT_SAMPLER_CUBE: return "GL_INT_SAMPLER_CUBE";
        case GL_INT_SAMPLER_2D_RECT: return "GL_INT_SAMPLER_2D_RECT";
        case GL_INT_SAMPLER_1D_ARRAY: return "GL_INT_SAMPLER_1D_ARRAY";
        case GL_INT_SAMPLER_2D_ARRAY: return "GL_INT_SAMPLER_2D_ARRAY";
        case GL_INT_SAMPLER_BUFFER: return "GL_INT_SAMPLER_BUFFER";
        case GL_INT_SAMPLER_2D_MULTISAMPLE: return "GL_INT_SAMPLER_2D_MULTISAMPLE";
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_INT_SAMPLER_CUBE_MAP_ARRAY: return "GL_INT_SAMPLER_CUBE_MAP_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_1D: return "GL_UNSIGNED_INT_SAMPLER_1D";
        case GL_UNSIGNED_INT_SAMPLER_2D: return "GL_UNSIGNED_INT_SAMPLER_2D";
        case GL_UNSIGNED_INT_SAMPLER_3D: return "GL_UNSIGNED_INT_SAMPLER_3D";
        case GL_UNSIGNED_INT_SAMPLER_CUBE: return "GL_UNSIGNED_INT_SAMPLER_CUBE";
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "GL_UNSIGNED_INT_SAMPLER_BUFFER";
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY";
        case GL_IMAGE_1D: return "GL_IMAGE_1D";
        case GL_IMAGE_2D: return "GL_IMAGE_2D";
        case GL_IMAGE_3D: return "GL_IMAGE_3D";
        case GL_IMAGE_2D_RECT: return "GL_IMAGE_2D_RECT";
        case GL_IMAGE_CUBE: return "GL_IMAGE_CUBE";
        case GL_IMAGE_BUFFER: return "GL_IMAGE_BUFFER";
        case GL_IMAGE_1D_ARRAY: return "GL_IMAGE_1D_ARRAY";
        case GL_IMAGE_2D_ARRAY: return "GL_IMAGE_2D_ARRAY";
        case GL_IMAGE_CUBE_MAP_ARRAY: return "GL_IMAGE_CUBE_MAP_ARRAY";
        case GL_IMAGE_2D_MULTISAMPLE: return "GL_IMAGE_2D_MULTISAMPLE";
        case GL_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_IMAGE_2D_MULTISAMPLE_ARRAY";
        case GL_INT_IMAGE_1D: return "GL_INT_IMAGE_1D";
        case GL_INT_IMAGE_2D: return "GL_INT_IMAGE_2D";
        case GL_INT_IMAGE_3D: return "GL_INT_IMAGE_3D";
        case GL_INT_IMAGE_2D_RECT: return "GL_INT_IMAGE_2D_RECT";
        case GL_INT_IMAGE_CUBE: return "GL_INT_IMAGE_CUBE";
        case GL_INT_IMAGE_BUFFER: return "GL_INT_IMAGE_BUFFER";
        case GL_INT_IMAGE_1D_ARRAY: return "GL_INT_IMAGE_1D_ARRAY";
        case GL_INT_IMAGE_2D_ARRAY: return "GL_INT_IMAGE_2D_ARRAY";
        case GL_INT_IMAGE_CUBE_MAP_ARRAY: return "GL_INT_IMAGE_CUBE_MAP_ARRAY";
        case GL_INT_IMAGE_2D_MULTISAMPLE: return "GL_INT_IMAGE_2D_MULTISAMPLE";
        case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY";
        case GL_UNSIGNED_INT_IMAGE_1D: return "GL_UNSIGNED_INT_IMAGE_1D";
        case GL_UNSIGNED_INT_IMAGE_2D: return "GL_UNSIGNED_INT_IMAGE_2D";
        case GL_UNSIGNED_INT_IMAGE_3D: return "GL_UNSIGNED_INT_IMAGE_3D";
        case GL_UNSIGNED_INT_IMAGE_2D_RECT: return "GL_UNSIGNED_INT_IMAGE_2D_RECT";
        case GL_UNSIGNED_INT_IMAGE_CUBE: return "GL_UNSIGNED_INT_IMAGE_CUBE";
        case GL_UNSIGNED_INT_IMAGE_BUFFER: return "GL_UNSIGNED_INT_IMAGE_BUFFER";
        case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: return "GL_UNSIGNED_INT_IMAGE_1D_ARRAY";
        case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: return "GL_UNSIGNED_INT_IMAGE_2D_ARRAY";
        case GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY: return "GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY";
        case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE";
        case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY";
        case GL_UNSIGNED_INT_ATOMIC_COUNTER: return "GL_UNSIGNED_INT_ATOMIC_COUNTER";
        default: return "Unknown";
    }
}
