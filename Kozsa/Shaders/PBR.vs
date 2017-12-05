#version 330 core

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);

	TexCoords = texCoords;
	WorldPos = vec3(model * vec4(position, 1.0f));
	Normal = mat3(model) * normal;
}