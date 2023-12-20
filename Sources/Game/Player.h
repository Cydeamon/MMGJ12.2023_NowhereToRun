//
// Created by cydeamon on 15.12.2023.
//

#ifndef NOWHERETORUN_PLAYER_H
#define NOWHERETORUN_PLAYER_H


#include "../Engine/Objects/GameObject.h"
#include "../Engine/Sprite.h"
#include "../Engine/Input/Input.h"

class Player : public GameObject
{
public:
    Player();

    void Update() override;
    void Draw() override;

private:
    Sprite *sprite;
    void pickAnimation();
    void handleMovement();
    void updateDirection();
    float speed = 50;
    int minX = 89;
    int maxX = 280;
    int minY = 43;
    int maxY = 205;
    void updateVelocity();
};


#endif //NOWHERETORUN_PLAYER_H
