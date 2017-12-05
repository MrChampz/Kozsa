#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 texCoords;

out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);

	WorldPos = vec3(model * vec4(position, 1.0));
	Normal = (inverse(transpose(model)) * vec4(normal, 0.0)).xyz;
	Tangent = tangent;
	BiTangent = bitangent;
	TexCoords = texCoords;
}