#version 450

layout(binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform ModelUBO {
    mat4 model;
} meshubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outPos; 
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * meshubo.model * vec4(inPosition, 1.0);
    outPos = vec3(meshubo.model * vec4(inPosition, 1.0));
    outNormal = inNormal;
    outTexCoord = inTexCoord;
}