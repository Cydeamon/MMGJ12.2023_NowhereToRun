//
// Created by cydeamon on 10.12.2023.
//

#include "Sprite.h"
#include "glm/fwd.hpp"
#include "glm/detail/type_mat4x4.hpp"
#include "GraphicLibraries/OpenGL.h"
#include "glm/ext/matrix_transform.hpp"

Sprite::Sprite(const char* path, Cygine::Color color)
{
    texture = new OpenGL::Texture(path);
    size = Cygine::Vector2(texture->GetWidth(), texture->GetHeight());
    this->color = color;
}

void Sprite::Update()
{

}

void Sprite::Draw()
{
    OpenGL::API::DrawSprite(this);
}

void Sprite::Move(float x, float y)
{
    position.x += x;
    position.y += y;
}

void Sprite::SetPosition(Sprite::Alignment alignment, int y)
{
    int x;

    switch(alignment)
    {
        case Alignment::BEGIN: x = 0; break;
        case Alignment::CENTER: x = OpenGL::API::GetInnerResolution().x / 2 - GetSize().x / 2; break;
        case Alignment::END: x = OpenGL::API::GetInnerResolution().x - GetSize().x; break;
        default:
            throw new std::runtime_error("Sprite::SetPosition: Invalid alignment");
    }

    position.x = x;
    position.y = y;
}

void Sprite::SetPosition(int x, Sprite::Alignment alignment)
{
    int y;

    switch(alignment)
    {
        case Alignment::BEGIN: y = 0; break;
        case Alignment::CENTER: y = OpenGL::API::GetInnerResolution().y / 2 - GetSize().y / 2; break;
        case Alignment::END: y = OpenGL::API::GetInnerResolution().y - GetSize().y; break;
        default:
            throw new std::runtime_error("Sprite::SetPosition: Invalid alignment");
    }

    position.x = x;
    position.y = y;
}

void Sprite::SetPosition(Sprite::Alignment alignmentX, Sprite::Alignment alignmentY)
{
    int x;
    int y;

    switch(alignmentX)
    {
        case Alignment::BEGIN: x = 0; break;
        case Alignment::CENTER: x = OpenGL::API::GetInnerResolution().x / 2 - GetSize().x / 2; break;
        case Alignment::END: x = OpenGL::API::GetInnerResolution().x - GetSize().x; break;
        default:
            throw new std::runtime_error("Sprite::SetPosition: Invalid alignment");
    }

    switch(alignmentY)
    {
        case Alignment::BEGIN: y = 0; break;
        case Alignment::CENTER: y = OpenGL::API::GetInnerResolution().y / 2 - GetSize().y / 2; break;
        case Alignment::END: y = OpenGL::API::GetInnerResolution().y - GetSize().y; break;
        default:
            throw new std::runtime_error("Sprite::SetPosition: Invalid alignment");
    }

    position.x = x;
    position.y = y;
}

void Sprite::SetPosition(int x, int y)
{
    position.x = x;
    position.y = y;
}

OpenGL::Texture* Sprite::GetTexture()
{
    return texture;
}
