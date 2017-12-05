#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexStartTime;
layout (location = 3) in vec3 VertexInitialVelocity;

out vec3 Position;
out vec3 Velocity;
out float StartTime;
out float Transp;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform float Time;
uniform float H;
uniform vec3 Accel;
uniform float ParticleLifetime;

void main()
{	
	Position = VertexPosition;
	Velocity = VertexVelocity;
	StartTime = VertexStartTime;

	if (Time >= StartTime)
	{
		float age = Time - StartTime;
		if (age > ParticleLifetime)
		{
			Position = vec3(0.0);
			Velocity = VertexInitialVelocity;
			StartTime = Time;
		}
		else
		{
			Position += Velocity * H;
			Velocity += Accel * H;
		}
	}
}