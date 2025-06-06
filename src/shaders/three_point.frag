#version 330 core

in vec2 texCoord;
in vec3 worldNormal;
in vec3 worldPos;

uniform sampler2D texture_diffuse1;
uniform bool useTexture;
uniform bool isSelected;

uniform bool keyEnabled;
uniform bool fillEnabled;
uniform bool backEnabled;

uniform vec3 keyPos;
uniform vec3 keyColor;
uniform float keyIntensity;

uniform vec3 fillPos;
uniform vec3 fillColor;
uniform float fillIntensity;

uniform vec3 backPos;
uniform vec3 backColor;
uniform float backIntensity;

uniform vec3 viewPos;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float q;

out vec4 FragColor;

float calculateAttenuation(float distance) {
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    
    return 1.0 / (constant + linear * distance + quadratic * (distance * distance));
}

vec3 calculatePointLight(vec3 lightPos, vec3 lightColor, float lightIntensity, 
                        vec3 normal, vec3 fragPos, vec3 viewPos, vec3 objectColor) {
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    float distance = length(lightPos - fragPos);
    float attenuation = calculateAttenuation(distance);
    
    vec3 ambient = ka * lightColor * lightIntensity;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = kd * diff * lightColor * lightIntensity * attenuation;
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), q);
    vec3 specular = ks * spec * lightColor * lightIntensity * attenuation;
    
    return (ambient + diffuse) * objectColor + specular;
}

void main()
{
    vec3 objectColor;
    if (useTexture) {
        objectColor = texture(texture_diffuse1, texCoord).rgb;
    } else {
        objectColor = vec3(0.8, 0.8, 0.8);
    }
    
    vec3 norm = normalize(worldNormal);
    
    vec3 result = vec3(0.0);
    
    if (keyEnabled) {
        vec3 keyContrib = calculatePointLight(keyPos, keyColor, keyIntensity, 
                                            norm, worldPos, viewPos, objectColor);
        result += keyContrib;
    }
    
    if (fillEnabled) {
        vec3 fillContrib = calculatePointLight(fillPos, fillColor, fillIntensity, 
                                             norm, worldPos, viewPos, objectColor);
        result += fillContrib;
    }
    
    if (backEnabled) {
        vec3 backContrib = calculatePointLight(backPos, backColor, backIntensity, 
                                             norm, worldPos, viewPos, objectColor);
        result += backContrib;
    }
    
    if (!keyEnabled && !fillEnabled && !backEnabled) {
        result = objectColor * ka * 0.2;
    }
    
    if (isSelected) {
        result = mix(result, vec3(1.0, 1.0, 0.0), 0.15);
    }
    
    FragColor = vec4(result, 1.0);
} 