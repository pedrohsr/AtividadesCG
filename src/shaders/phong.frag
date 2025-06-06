#version 330 core

in vec2 texCoord;
in vec3 vNormal;
in vec4 fragPos;

uniform sampler2D texture_diffuse1;
uniform bool useTexture;
uniform bool isSelected;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float q;

out vec4 FragColor;

void main()
{
    vec3 objectColor;
    if (useTexture) {
        objectColor = texture(texture_diffuse1, texCoord).rgb;
    } else {
        objectColor = vec3(0.8, 0.8, 0.8); // Cor padrão cinza
    }

    vec3 ambient = ka * lightColor;

    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPos - vec3(fragPos));
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = kd * diff * lightColor;

    vec3 R = normalize(reflect(-L, N));
    vec3 V = normalize(viewPos - vec3(fragPos));
    float spec = max(dot(R, V), 0.0);
    spec = pow(spec, q);
    vec3 specular = ks * spec * lightColor;

    vec3 result = (ambient + diffuse) * objectColor + specular;

    if (isSelected) {
        result = mix(result, vec3(1.0, 1.0, 0.0), 0.2);
    }

    FragColor = vec4(result, 1.0);
} 