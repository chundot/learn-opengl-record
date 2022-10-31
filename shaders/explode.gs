#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float curTime;

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0;
    vec3 dir = normal * ((sin(curTime) + 1.0) / 2.0) * magnitude;
    return position + vec4(dir, 0.0);
}

vec3 getNormal() {
    return normalize(cross(vec3(gl_in[0].gl_Position - gl_in[1].gl_Position), vec3(gl_in[2].gl_Position - gl_in[1].gl_Position)));
}

void main() {
    vec3 normal = getNormal();
    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
}