//
// Created by cydeamon on 10.12.2023.
//

#ifndef NOWHERETORUN_GAMEOBJECT_H
#define NOWHERETORUN_GAMEOBJECT_H


#include "../Math/Vector2.h"

class GameObject
{
public:
    virtual void Update() = 0;
    virtual void Draw() = 0;

    Cygine::Vector2 GetPosition() const { return position; }
    Cygine::Vector2 GetDirection() const { return direction; }
    Cygine::Vector2 GetVelocity() const { return velocity; }

    void SetPosition(Cygine::Vector2 position) { this->position = position; }
    void SetDirection(Cygine::Vector2 direction) { this->direction = direction; }
    void SetVelocity(Cygine::Vector2 velocity) { this->velocity = velocity; }

protected:
    Cygine::Vector2 position;
    Cygine::Vector2 direction;
    Cygine::Vector2 velocity;
};


#endif //NOWHERETORUN_GAMEOBJECT_H
