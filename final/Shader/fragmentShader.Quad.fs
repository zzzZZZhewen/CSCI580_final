#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec2 ndcPos;

uniform sampler2D screenTexture;
uniform sampler2D effectTexture;
//uniform vec3 lightPosNDC;

void main()
{
    
    vec3 col = texture(screenTexture, TexCoords).rgb + texture(effectTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
} 