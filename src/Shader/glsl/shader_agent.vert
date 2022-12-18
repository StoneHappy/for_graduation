#version 450
const int MAX_BONES = 70; // Max number of bones

layout(binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform ModelUBO {
    mat4 model;
	mat4 bones[MAX_BONES]; // Bone transformations 
} meshubo;

layout(location = 0) in vec3	inPosition;
layout(location = 1) in vec3	inNormal;
layout(location = 2) in vec2	inTexCoord;
layout(location = 3) in ivec4	inBoneIDs; // Stream of vertex bone IDs
layout(location = 4) in vec4	inWeights; // Stream of vertex weights

layout(location = 0) out vec3 outPos; 
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;


void main()
{
	// Multiply each bone transformation by the particular weight
	// and combine them. 
   	mat4 BoneTransform = meshubo.bones[ inBoneIDs[0] ] * inWeights[0];
	BoneTransform += meshubo.bones[ inBoneIDs[1] ] * inWeights[1];
    BoneTransform += meshubo.bones[ inBoneIDs[2] ] * inWeights[2];
    BoneTransform += meshubo.bones[ inBoneIDs[3] ] * inWeights[3];

	// Transformed vertex position 
	vec4 tPos = BoneTransform * vec4(inPosition, 1.0);

	gl_Position = ubo.proj * ubo.view * meshubo.model * tPos;

	// Transformed normal 
	vec4 outNormal = BoneTransform * vec4(inNormal, 0.0);

	outTexCoord = inTexCoord;
}