//
// Created by cydeamon on 10.12.2023.
//

#ifndef NOWHERETORUN_SPRITE_H
#define NOWHERETORUN_SPRITE_H

#include <string>
#include <map>
#include "Objects/GameObject.h"
#include "GraphicLibraries/Texture.h"
#include "Math/Vector2.h"
#include "Color.h"

class Sprite : public GameObject
{
public:
    enum Alignment
    {
        BEGIN, END, CENTER
    };

    Sprite(std::string path, Cygine::Color color = Cygine::Color::White());

    void Update() override;
    void Draw() override;
    void Move(float x, float y);
    void PlayAnimation(std::string name);
    void StopAnimation();
    void PauseAnimation();
    void ResumeAnimation();

    void AddSpriteAnimation(std::string name, int startFrame, int endFrame, bool isLooping = true, bool isBackward = false);
    void SetPosition(Alignment alignment, int y);
    void SetPosition(int x, Alignment alignment);
    void SetPosition(Alignment alignmentX, Alignment alignmentY);
    void SetPosition(int x, int y);
    void SetPosition(Cygine::Vector2 position) { this->position = position; }
    void SetFrameSize(int width, int height);
    void SetRotation(float rotation) { this->rotation = rotation; }
    void SetColor(Cygine::Color color) { this->color = color; }
    OpenGL::Texture *GetTexture() { return texture; }
    void SetAnimated(bool animated) { isAnimated = animated; }

    Cygine::Vector2 GetSize() const { return size; }
    float GetRotation() const { return rotation; }
    Cygine::Color GetColor() const { return color; }
    Cygine::Vector2 GetFrameSize() { return frameSize; }
    int GetCurrentFrame() { return currentFrame; }
    int GetSpriteSheetColumnsNumber() { return spriteSheetColumns; }
    int GetSpriteSheetRowsNumber() { return spriteSheetRows; }
    bool IsAnimated() { return isAnimated; }

private:
    struct SpriteAnimation
    {
        int startFrame, endFrame;
        bool isLooping;
        bool isBackward;
    };

    OpenGL::Texture *texture;
    Cygine::Vector2 size;
    Cygine::Vector2 frameSize;
    static float vertices[];
    float rotation = 0.0f;
    bool isAnimated = false;
    int currentFrame = 0;
    int spriteSheetColumns = 1;
    int spriteSheetRows = 1;
    int totalFrames = 0;
    float frameTime = 1.0f / 12.0f;
    float currentFrameTime = 0;
    Cygine::Color color = Cygine::Color::White();
    void calculateFramesProperties();
    std::map<std::string, SpriteAnimation> animations;
    std::string currentAnimationName = "";
    SpriteAnimation *currentAnimation = nullptr;
    bool isPlaying = false;
};


#endif //NOWHERETORUN_SPRITE_H
