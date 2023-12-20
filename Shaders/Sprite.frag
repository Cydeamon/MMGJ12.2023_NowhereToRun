#version 460 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform vec2 uvOffset;
uniform vec2 framesNum;

void main()
{
        float x = 1.0 / framesNum.x;
        float y = 1.0 / framesNum.y;

        color = spriteColor * texture(
            image,
            vec2(TexCoords.x * x, TexCoords.y * y) + vec2(x * uvOffset.x, y * uvOffset.y)
        );
}