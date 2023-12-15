//
// Created by cydeamon on 07.12.2023.
//

#include "Texture.h"
#include "OpenGL.h"

namespace OpenGL
{
    GLuint Texture::lastTextureUnit = GL_TEXTURE0;

    Texture::Texture(const char* path)
    {
        textureUnit = lastTextureUnit + 1;
        lastTextureUnit = textureUnit;
        glActiveTexture(lastTextureUnit);

        try
        {
            // Check if file exists
            std::ifstream file(path);
            if (!file.good())
                throw std::runtime_error("File not found: " + std::string(path));

            // Load image
            unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

            if (!data)
                throw std::runtime_error("Failed to load texture");

            int format = nrChannels == 3 ? GL_RGB : GL_RGBA;        

            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
            API::CheckErrors();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            std::cout << "Loaded texture: " << path << std::endl;
            std::cout << "id: " << id << " textureUnit: " << textureUnit << std::endl;
            std::cout << "lastTextureUnit: " << lastTextureUnit << std::endl;

            glActiveTexture(GL_TEXTURE0);
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("Failed to load texture: " + std::string(e.what()));
        }
    }
} // OpenGL