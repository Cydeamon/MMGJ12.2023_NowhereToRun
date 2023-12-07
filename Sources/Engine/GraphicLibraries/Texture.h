//
// Created by cydeamon on 07.12.2023.
//

#ifndef NOWHERETORUN_TEXTURE_H
#define NOWHERETORUN_TEXTURE_H

#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace OpenGL
{
    class Texture
    {
    public:
        Texture(const char* path);

        // Getters
        int GetWidth() { return width; }
        int GetHeight() { return height; }
        int GetNrChannels() { return nrChannels; }
        GLuint GetID() { return id; }

    private:
        int width;
        int height;
        int nrChannels;
        GLuint id;
    };

} // OpenGL

#endif //NOWHERETORUN_TEXTURE_H
