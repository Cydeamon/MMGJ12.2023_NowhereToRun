//
// Created by cydeamon on 17.12.2023.
//

#include "GraphicLibraries/OpenGL.h"

class Shape
{
public:
    virtual void Draw()
    {
        OpenGL::API::DrawShape(this);
    }

    Cygine::Vector2 position;
    Cygine::Color color = Cygine::Color::White();
};

class Rectangle : public Shape
{
public:
    Rectangle(float x, float y, float width, float height)
    {
        this->position = Cygine::Vector2(x, y);
        this->size = Cygine::Vector2(width, height);
    }

    Cygine::Vector2 size;
};

class Circle : public Shape
{
public:
    Circle(float x, float y, float radius)
    {
        this->position = Cygine::Vector2(x, y);
        this->radius = radius;
    }

    float radius;
};

class Line : public Shape
{
public:
    Line(float x1, float y1, float x2, float y2, int thickness = 1)
    {
        this->position = Cygine::Vector2(x1, y1);
        this->end = Cygine::Vector2(x2, y2);
        this->thickness = thickness;
    }

    Cygine::Vector2 end;
    int thickness;

    float GetAngle() { return atan2(end.y - position.y, end.x - position.x); }
    float GetAngleInDegrees() { return atan2(end.y - position.y, end.x - position.x) * 180.0f / M_PI; }
};