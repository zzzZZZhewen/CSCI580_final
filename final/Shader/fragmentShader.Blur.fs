#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec2 ndcPos;

uniform sampler2D screenTexture;
uniform vec3 lightPosNDC;

void main()
{
    float samplerScale = 0.05;
    int RADIAL_SAMPLE_COUNT = 6;
    vec3 col = texture(screenTexture, TexCoords.xy).rgb;
    vec2 offsetBlur =  (vec2(lightPosNDC.x, lightPosNDC.y) - ndcPos);
    //if (length(offsetBlur) < 0.8)

        col = vec3(0.0,0.0,0.0);

        for (int i = 0; i < RADIAL_SAMPLE_COUNT; i ++)
        {
            col = col + texture(screenTexture, TexCoords.xy + offsetBlur * i * samplerScale).rgb;
        }
        col = col / RADIAL_SAMPLE_COUNT;
        
        FragColor = vec4(col, 1.0);

} 