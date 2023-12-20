#version 460

out vec4 FragColor;

/*
 * Shape types:
 * 0 - Circle
 * 1 - Line
 * 2 - Rectangle
 */
uniform int shapeType;

uniform vec2 pos;
uniform float circleRadius;
uniform vec2 lineEndPosition;
uniform int lineThickness;
uniform vec2 rectWidthHeight;
uniform vec4 color;

void main()
{
    if (shapeType == 0)
    {
        // Circle
        float dist = distance(gl_FragCoord.xy, pos);

        if (dist < circleRadius)
            FragColor = color;
        else
            discard;
    }

    if (shapeType == 1)
    {
        // Line
        if (distance(gl_FragCoord.xy, lineEndPosition) < lineThickness)
            FragColor = vec4(1, 0, 0, 1);
    }

    if (shapeType == 2)
    {
        // Rectangle
        FragColor = color;
    }
}