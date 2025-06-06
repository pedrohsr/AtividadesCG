#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Material properties structure
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform sampler2D texture_diffuse1;
uniform Material material;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool useTexture;
uniform vec3 objectColor;

void main()
{
    if (useTexture) {
        // Simple texture display to verify texture mapping
        vec3 texColor = texture(texture_diffuse1, TexCoord).rgb;
        
        // Basic lighting calculation
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(vec3(5.0, 5.0, 5.0) - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        
        // Combine texture with simple lighting
        vec3 ambient = 0.3 * texColor;
        vec3 diffuse = diff * texColor;
        
        FragColor = vec4(ambient + diffuse, 1.0);
    } else {
        // Fallback for non-textured objects
        FragColor = vec4(objectColor, 1.0);
    }
} 