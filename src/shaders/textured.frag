#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool useTexture;
uniform vec3 objectColor;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightColor_local = vec3(1.0, 1.0, 1.0);
    vec3 lightPos_local = vec3(5.0, 5.0, 5.0);
    vec3 lightDir = normalize(lightPos_local - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor_local;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor_local;
    
    vec3 result;
    if (useTexture) {
        vec3 texColor = texture(texture_diffuse1, TexCoord).rgb;
        result = (ambient + diffuse + specular) * texColor;
    } else {
        result = (ambient + diffuse + specular) * objectColor;
    }
    
    FragColor = vec4(result, 1.0);
} 