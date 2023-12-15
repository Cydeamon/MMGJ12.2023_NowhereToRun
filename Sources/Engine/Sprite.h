//
// Created by cydeamon on 10.12.2023.
//

#ifndef NOWHERETORUN_SPRITE_H
#define NOWHERETORUN_SPRITE_H

#include <string>
#include "Objects/GameObject.h"
#include "GraphicLibraries/Texture.h"
#include "Math/Vector2.h"
#include "Color.h"

class Sprite : public GameObject
{
public:
    enum Alignment { BEGIN, END, CENTER };

    Sprite(const char* path, Cygine::Color color = Cygine::Color::White());

    void Update() override;
    void Draw() override;
    void Move(float x, float y);

    Cygine::Vector2 GetSize() const { return size; }
    float GetRotation() const { return rotation; }
    Cygine::Color GetColor() const { return color; }

    void SetPosition(Alignment alignment, int y);
    void SetPosition(int x, Alignment alignment);
    void SetPosition(Alignment alignmentX, Alignment alignmentY);
    void SetPosition(int x, int y);

    OpenGL::Texture* GetTexture();
private:
    OpenGL::Texture* texture;
    Cygine::Vector2 size;

    static float vertices[];
    float rotation = 0.0f;
    Cygine::Color color = Cygine::Color::White();
};


#endif //NOWHERETORUN_SPRITE_H
