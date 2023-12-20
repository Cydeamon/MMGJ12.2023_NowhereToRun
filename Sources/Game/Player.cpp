//
// Created by cydeamon on 15.12.2023.
//

#include "Player.h"
#include "../Engine/GraphicLibraries/OpenGL.h"

Player::Player()
{
    sprite = new Sprite("Assets/Characters/Player.png");
    sprite->SetAnimated(true);
    sprite->SetFrameSize(8, 8);

    position = Cygine::Vector2(50, 50);
    sprite->SetPosition(50, 50);

    // Setup animations
    sprite->AddSpriteAnimation("Idle", 0, 5);
    sprite->AddSpriteAnimation("Run Right", 6, 11);
    sprite->AddSpriteAnimation("Run Left", 12, 17);
    sprite->AddSpriteAnimation("Run Down", 18, 22);
}

void Player::Update()
{
    updateDirection();
    updateVelocity();
    handleMovement();

    pickAnimation();
    sprite->Update();
}

void Player::Draw()
{
    sprite->Draw();
}

void Player::pickAnimation()
{
    if (velocity == Cygine::Vector2::Zero)
    {
        sprite->PlayAnimation("Idle");
    }
    else if (velocity.x > 0)
    {
        sprite->PlayAnimation("Run Right");
    }
    else if (velocity.x < 0)
    {
        sprite->PlayAnimation("Run Left");
    }
    else
    {
        sprite->PlayAnimation("Run Down");
    }
}

void Player::handleMovement()
{
    position += velocity * OpenGL::API::GetDeltaTime();

    if (position.x < minX)
        position.x = minX;
    if (position.x > maxX)
        position.x = maxX;
    if (position.y < minY)
        position.y = minY;
    if (position.y > maxY)
        position.y = maxY;

    sprite->SetPosition(position);
}

void Player::updateDirection()
{
    // Update direction
    direction = Cygine::Vector2::Zero;

    if (Cygine::Input::IsPressed(Cygine::KeyboardKey::A))
        direction.x -= 1;
    if (Cygine::Input::IsPressed(Cygine::KeyboardKey::D))
        direction.x += 1;
    if (Cygine::Input::IsPressed(Cygine::KeyboardKey::W))
        direction.y -= 1;
    if (Cygine::Input::IsPressed(Cygine::KeyboardKey::S))
        direction.y += 1;

    direction.Normalize();
}

void Player::updateVelocity()
{
    velocity = direction * speed;
}
