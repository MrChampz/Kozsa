#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraRight;
uniform vec3 cameraUp;

uniform vec3 offset;
uniform vec3 scale;

void main()
{
	vec3 pos = offset + cameraRight * (position.x - 0.5) * scale.x + cameraUp * (position.y - 0.5) * scale.y;
	gl_Position = projection * view * vec4(pos, 1.0);

	TexCoords = texCoords;
}