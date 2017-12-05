#version 330 core

in vec3 Normal;
in vec2 TexCoords;

out vec4 Color;

uniform sampler2D texture_diffuse1;

void main()
{
	Color = texture(texture_diffuse1, TexCoords);
}