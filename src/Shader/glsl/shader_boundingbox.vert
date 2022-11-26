#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


vec3 positions[8] = vec3[](
    vec3(   -1.0,  -1.0,        1.00),
    vec3(   -1.0,  1.00,        1.00),
    vec3(   -1.0,  -1.0,        -1.0),
    vec3(   -1.0,  1.0,         -1.0),
    vec3(   1.0,   -1.0,        1.0),
    vec3(   1.0,   1.0,         1.0),
    vec3(   1.0,   -1.0,        -1.0),
    vec3(   1.0,   1.0,         -1.0)
);

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(positions[gl_VertexIndex] * 0.5, 1.0);
}