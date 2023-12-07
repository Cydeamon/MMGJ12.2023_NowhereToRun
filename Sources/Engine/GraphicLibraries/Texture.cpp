//
// Created by cydeamon on 07.12.2023.
//

#include "Texture.h"
#include "OpenGL.h"

namespace OpenGL
{
    Texture::Texture(const char* path)
    {
        try 
        {
            unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

            if (!data)
                throw std::runtime_error("Failed to load texture");

            int format = nrChannels == 3 ? GL_RGB : GL_RGBA;        

            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
            API::CheckErrors();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            std::cout << "Loaded texture: " << path << " (" << id << ")" << std::endl;
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("Failed to load texture: " + std::string(e.what()));
        }
    }
} // OpenGL