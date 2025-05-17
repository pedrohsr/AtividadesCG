#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform bool wireframe;

void main() {
    if (wireframe) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);  // Black color for wireframe
    } else {
        vec3 lightPos = vec3(5.0, 5.0, 5.0);
        vec3 lightColor = vec3(1.0, 1.0, 1.0);
        
        // Ambient
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * lightColor;
        
        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        vec3 result = (ambient + diffuse) * objectColor;
        FragColor = vec4(result, 1.0);
    }
} 