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
