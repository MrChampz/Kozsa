#include "ConeParticleEmitterObject.h"

ConeParticleEmitterObject::ConeParticleEmitterObject(Texture2D texture, int count, glm::vec3 position,glm::vec3 size, glm::vec3 velocity)
	: ParticleEmitterObject(texture, count, position, size, velocity)
{
}

ConeParticleEmitterObject::~ConeParticleEmitterObject()
{
	// Delete the VBO and VAO
	glDeleteBuffers(1, &this->mVBO);
	glDeleteVertexArrays(1, &this->mVAO);
}


void ConeParticleEmitterObject::Update(GLfloat dt)
{
	// Add new particles 
	for (GLuint i = 0; i < this->mNewParticlesCount; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->mParticles[unusedParticle]);
	}

	// Update all particles
	for (GLuint i = 0; i < this->mParticlesCount; ++i)
	{
		Particle &p = this->mParticles[i];
		p.Life -= dt;		// Reduce life
		if(p.Life > 0.0f)
		{
			p.Position	-= p.Velocity * dt;
			p.Color.a	-= dt * 0.2;
		}
	}
}

void ConeParticleEmitterObject::respawnParticle(Particle& particle)
{
	this->mParticlesVelocity = glm::vec3(0.0f, ((float)rand() / RAND_MAX) - 1.0f, 0.0f);

	particle.Position = this->mParticlesOffset;
	particle.Velocity = (this->mVelocity + this->mParticlesVelocity) / particle.Weight;
	particle.Color = glm::vec4(1.0f);
	particle.Size = glm::vec3(this->mParticlesSize);
	particle.Weight = 1.0f;
	particle.Life = 1.0f;
}