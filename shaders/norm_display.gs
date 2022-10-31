#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.04;

uniform mat4 proj;

void GenerateLine(int idx) {
    gl_Position = proj * gl_in[idx].gl_Position;
    EmitVertex();
    gl_Position = proj * (gl_in[idx].gl_Position + vec4(gs_in[idx].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main() {
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}