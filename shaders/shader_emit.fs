#version 330 core

in vec2 TexCoords;
in vec3 ourColor;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emit;
    float shininess;
}; 

uniform Material material;
uniform Light light;
uniform float mixValue;
uniform vec3 viewPos;

void main()
{    
    // 漫反射 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // 环境光
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // 放射光
    vec3 emit = vec3(texture(material.emit, TexCoords));
    // 最终计算
    vec3 result = ambient + diffuse + specular + emit;
    FragColor = vec4(result, 1.0);
}