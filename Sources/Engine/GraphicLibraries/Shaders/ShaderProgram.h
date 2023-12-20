//
// Created by cydeamon on 06.12.2023.
//

#ifndef NOWHERETORUN_SHADERPROGRAM_H
#define NOWHERETORUN_SHADERPROGRAM_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include "ShaderVec4.h"
#include "../../Math/Vector2.h"
#include <string>
#include <map>
#include <stdexcept>

namespace OpenGL
{
    class ShaderProgram
    {
    public:
        ShaderProgram(std::string name);
        ~ShaderProgram();

        void Use();
        void SetVertexShader(int shaderID);
        void SetFragmentShader(int shaderID);

        void SetUniformValue(std::string name, ShaderVec4 value);
        void SetUniformValue(std::string name, float value);
        void SetUniformValue(std::string name, int value);
        void SetUniformValue(std::string name, GLuint value) { SetUniformValue(name, (int)value); }
        void SetUniformValue(std::string name, glm::mat4 value);
        void SetUniformValue(std::string name, glm::vec2 value);
        void SetUniformValue(std::string name, Cygine::Vector2 value);

        std::string GetName() const { return name; }
        unsigned int GetID() const { return id; }
        unsigned int GetVertexShader() const { return vertexShader; }
        unsigned int GetFragmentShader() const { return fragmentShader; }

    private:
        std::string name;
        unsigned int id;
        unsigned int vertexShader = -1;
        unsigned int fragmentShader = -1;
        void checkLinkingErrors();
        std::map <std::string, int> uniforms;

        void updateUniforms();
        std::string getTypeName(GLenum type);
        GLenum getUniformType(const std::string &name);
    };
}

#endif //NOWHERETORUN_SHADERPROGRAM_H
