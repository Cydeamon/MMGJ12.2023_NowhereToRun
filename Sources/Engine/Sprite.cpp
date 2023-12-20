//
// Created by cydeamon on 10.12.2023.
//

#include "Sprite.h"
#include "glm/fwd.hpp"
#include "glm/detail/type_mat4x4.hpp"
#include "GraphicLibraries/OpenGL.h"
#include "glm/ext/matrix_transform.hpp"

Sprite::Sprite(std::string path, Cygine::Color color)
{
    texture = new OpenGL::Texture(path.c_str());
    size = Cygine::Vector2(texture->GetWidth(), texture->GetHeight());
    frameSize = size;
    this->color = color;
}

void Sprite::Update()
{
    if (isAnimated && isPlaying)
    {
        currentFrameTime += OpenGL::API::GetDeltaTime();

        if (currentFrameTime >= frameTime)
        {
            currentFrameTime = 0;

            if (currentAnimation)
            {
                if (currentAnimation->isBackward)
                {
                    currentFrame--;

                    if (currentFrame < currentAnimation->startFrame)
                    {
                        if (currentAnimation->isLooping)
                            currentFrame = currentAnimation->endFrame;
                        else
                            StopAnimation();
                    }
                }
                else
                {
                    currentFrame++;

                    if (currentFrame > currentAnimation->endFrame)
                    {
                        if (currentAnimation->isLooping)
                            currentFrame = currentAnimation->startFrame;
                        else
                            StopAnimation();
                    }
                }
            }
            else
            {
                currentFrame++;

                if (currentFrame >= totalFrames)
                    currentFrame = 0;
            }
        }
    }
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

    switch (alignment)
    {
        case Alignment::BEGIN: x = 0;
            break;
        case Alignment::CENTER: x = OpenGL::API::GetInnerResolution().x / 2 - GetSize().x / 2;
            break;
        case Alignment::END: x = OpenGL::API::GetInnerResolution().x - GetSize().x;
            break;
        default: throw std::runtime_error("Sprite::SetPosition: Invalid alignment");
    }

    position.x = x;
    position.y = y;
}

void Sprite::SetPosition(int x, Sprite::Alignment alignment)
{
    int y;

    switch (alignment)
    {
        case Alignment::BEGIN: y = 0;
            break;
        case Alignment::CENTER: y = OpenGL::API::GetInnerResolution().y / 2 - GetSize().y / 2;
            break;
        case Alignment::END: y = OpenGL::API::GetInnerResolution().y - GetSize().y;
            break;
        default:throw std::runtime_error("Sprite::SetPosition: Invalid alignment");
    }

    position.x = x;
    position.y = y;
}

void Sprite::SetPosition(Sprite::Alignment alignmentX, Sprite::Alignment alignmentY)
{
    int x;
    int y;

    switch (alignmentX)
    {
        case Alignment::BEGIN: x = 0;
            break;
        case Alignment::CENTER: x = OpenGL::API::GetInnerResolution().x / 2 - GetSize().x / 2;
            break;
        case Alignment::END: x = OpenGL::API::GetInnerResolution().x - GetSize().x;
            break;
        default:throw std::runtime_error("Sprite::SetPosition: Invalid alignment");
    }

    switch (alignmentY)
    {
        case Alignment::BEGIN: y = 0;
            break;
        case Alignment::CENTER: y = OpenGL::API::GetInnerResolution().y / 2 - GetSize().y / 2;
            break;
        case Alignment::END: y = OpenGL::API::GetInnerResolution().y - GetSize().y;
            break;
        default:throw std::runtime_error("Sprite::SetPosition: Invalid alignment");
    }

    position.x = x;
    position.y = y;
}

void Sprite::SetPosition(int x, int y)
{
    position.x = x;
    position.y = y;
}

void Sprite::SetFrameSize(int width, int height)
{
    frameSize = Cygine::Vector2(width, height);
    calculateFramesProperties();
}

void Sprite::calculateFramesProperties()
{
    spriteSheetColumns = texture->GetWidth() / frameSize.x;
    spriteSheetRows = texture->GetHeight() / frameSize.y;
    totalFrames = spriteSheetColumns * spriteSheetRows;
}

void Sprite::AddSpriteAnimation(std::string name, int startFrame, int endFrame, bool isLooping, bool isBackward)
{
    // Check for errors
    if (animations.count(name) > 0)
        throw std::runtime_error("Sprite::AddSpriteAnimation: Animation with name '" + name + "' already exists");

    if (startFrame < 0 || startFrame >= totalFrames)
        throw std::runtime_error("Sprite::AddSpriteAnimation: Start frame out of bounds. Total frames: " + std::to_string(totalFrames));

    if (endFrame < 0 || endFrame >= totalFrames)
        throw std::runtime_error("Sprite::AddSpriteAnimation: End frame out of bounds. Total frames: " + std::to_string(totalFrames));

    if (startFrame > endFrame)
        throw std::runtime_error("Sprite::AddSpriteAnimation: Start frame is greater than end frame");

    // Add to animations
    animations[name] = SpriteAnimation{startFrame, endFrame, isLooping, isBackward};
}

void Sprite::PlayAnimation(std::string name)
{
    if (animations.count(name) == 0)
        throw std::runtime_error("Sprite::PlayAnimation: Animation with name '" + name + "' does not exist");

    if (currentAnimationName != name)
    {
        currentAnimationName = name;
        currentAnimation = &animations[name];
        currentFrame = currentAnimation->isBackward ? currentAnimation->endFrame : currentAnimation->startFrame;
        currentFrameTime = 0;
        isPlaying = true;
    }
}

void Sprite::StopAnimation()
{
    currentFrame = 0;
    currentFrameTime = 0;
    isPlaying = false;
    currentAnimation = nullptr;
    currentAnimationName = "";
}

void Sprite::PauseAnimation()
{
    isPlaying = false;
    currentFrameTime = 0;
}

void Sprite::ResumeAnimation()
{
    isPlaying = true;
}
