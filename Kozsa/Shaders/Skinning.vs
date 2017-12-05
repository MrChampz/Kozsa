#version 330 core

layout (location = 0) in vec3   position;
layout (location = 1) in vec3   normal;
layout (location = 2) in vec3	tangent;
layout (location = 3) in vec3	bitangent;
layout (location = 4) in vec2   texCoords;
layout (location = 5) in vec4	boneIndices1;
layout (location = 6) in vec4	boneIndices2;
layout (location = 7) in vec4   boneWeights1;
layout (location = 8) in vec4   boneWeights2;

out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define MAX_BONES 100
uniform mat4 Bones[MAX_BONES];

void main()
{
	mat4 boneMat = mat4(1);
	if (length(boneWeights1 + boneWeights2) > 0) {
		boneMat = boneWeights1[0] * Bones[int(boneIndices1[0])]
				+ boneWeights1[1] * Bones[int(boneIndices1[1])]
				+ boneWeights1[2] * Bones[int(boneIndices1[2])]
				+ boneWeights1[3] * Bones[int(boneIndices1[3])]
				+ boneWeights2[0] * Bones[int(boneIndices2[0])]
				+ boneWeights2[1] * Bones[int(boneIndices2[1])]
				+ boneWeights2[2] * Bones[int(boneIndices2[2])]
				+ boneWeights2[3] * Bones[int(boneIndices2[3])];
	}

	gl_Position = projection * view * model * boneMat * vec4(position, 1.0);

	WorldPos = vec3(model * boneMat * vec4(position, 1.0f));
	Normal = (inverse(transpose(model)) * boneMat * vec4(normal, 0.0)).xyz;
	Tangent = tangent;
	BiTangent = bitangent;
	TexCoords = texCoords;
}