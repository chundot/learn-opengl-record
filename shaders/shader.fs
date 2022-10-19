#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture0;
uniform sampler2D ourTexture1;

void main() {
    FragColor = mix(texture(ourTexture0, TexCoord), texture(ourTexture1, TexCoord), 0.2);
}