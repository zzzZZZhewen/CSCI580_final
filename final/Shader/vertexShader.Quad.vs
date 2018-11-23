#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 ndcPos;

uniform float moveX;
uniform float moveY;

void main()
{
    TexCoords = aTexCoords;
    ndcPos = aPos;
    if (moveX == 0)
    {
        gl_Position = vec4(aPos.x, aPos.y, -1.0, 1.0);    
    }
    else
    {
        gl_Position = vec4(aPos.x * 0.5 + moveX, aPos.y * 0.5 + moveY, -1.0, 1.0); 
    }
    
}  