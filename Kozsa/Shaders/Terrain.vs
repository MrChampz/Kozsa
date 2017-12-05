#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 binormal;
layout (location = 4) in vec2 texCoords;
layout (location = 5) in vec3 color;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out mat3 TBN;
out vec3 Color;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);

	TexCoords = texCoords * 15.0;
	WorldPos = vec3(model * vec4(position, 1.0));
	Color = color;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * binormal);
    vec3 N = normalize(normalMatrix * normal);    
    TBN = transpose(mat3(T, B, N));
    Normal = N;
}