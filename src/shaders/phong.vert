#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 aNormal;   
layout (location = 2) in vec2 aTexCoord; 

uniform mat4 model;     
uniform mat4 view;       
uniform mat4 projection; 

out vec2 texCoord;
out vec3 vNormal;
out vec4 fragPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    fragPos = model * vec4(position, 1.0);
    texCoord = aTexCoord;
    vNormal = aNormal;
} 