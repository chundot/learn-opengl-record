#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec3 spNormal;

uniform mat4 model;
layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    TexCoords = aTexCoords;
    spNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
}