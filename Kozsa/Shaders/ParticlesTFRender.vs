#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexStartTime;
layout (location = 3) in vec3 VertexInitialVelocity;

out float Transp;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform float Time;
uniform float ParticleLifetime;

void main()
{	
	float age = Time - VertexStartTime;
	Transp = 1.0 - age / ParticleLifetime;

	gl_Position = projection * view * model * vec4(VertexPosition, 1.0);
}