#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 ambientLightColor;
uniform float ambientLightIntensity;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float lightIntensity;

uniform vec3 viewPos;
uniform float specularStrength;
uniform int shininess;

uniform sampler2D texture_diffuse1;

void main()
{    

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 objectColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 ambient = ambientLightColor * ambientLightIntensity;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}