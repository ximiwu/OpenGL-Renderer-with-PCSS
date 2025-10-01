#version 430
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoord;

void main()
{
    texCoord = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}