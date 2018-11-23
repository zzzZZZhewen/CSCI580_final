#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 ndcPos;

void main()
{
    TexCoords = aTexCoords;
    ndcPos = aPos;
    gl_Position = vec4(aPos.x, aPos.y, -1.0, 1.0); 
}  