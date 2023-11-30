#version 460

layout (location = 0) in vec3 vertPos;

void main()
{
    gl_Position = vec4(vertPos.x, vertPos.y, vertPos.z, 1.0);
}
