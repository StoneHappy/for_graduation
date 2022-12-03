#version 450

layout(binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} ubo;


vec3 positions[24] = vec3[](
    // first square
    vec3(   -1.0,   -1.0,   -1.0),
    vec3(   1.0,    -1.0,   -1.0),
    vec3(   1.0,    -1.0,   -1.0),
    vec3(   1.0,    1.0,    -1.0),
    vec3(   1.0,    1.0,    -1.0),
    vec3(   -1.0,   1.0,    -1.0),
    vec3(   -1.0,   1.0,    -1.0),
    vec3(   -1.0,   -1.0,   -1.0),

    // second square
    vec3(   -1.0,   -1.0,   1.0),
    vec3(   1.0,    -1.0,   1.0),
    vec3(   1.0,    -1.0,   1.0),
    vec3(   1.0,    1.0,    1.0),
    vec3(   1.0,    1.0,    1.0),
    vec3(   -1.0,   1.0,    1.0),
    vec3(   -1.0,   1.0,    1.0),
    vec3(   -1.0,   -1.0,   1.0),

    // bridges
    vec3(   -1.0,  -1.0, -1.0),
    vec3(   -1.0,  -1.0, 1.0),

    vec3(   1.0,  -1.0, -1.0),
    vec3(   1.0,  -1.0, 1.0),

    vec3(   1.0,  1.0, -1.0),
    vec3(   1.0,  1.0, 1.0),

    vec3(   -1.0,  1.0, -1.0),
    vec3(   -1.0,  1.0, 1.0)
);
void main() {
    gl_Position = ubo.proj * ubo.view * vec4(positions[gl_VertexIndex] * 0.5, 1.0);
}