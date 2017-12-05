#pragma once

#include "ParticleEmitterObject.h"

class ConeParticleEmitterObject : public ParticleEmitterObject
{
public:
	// Constructor
	ConeParticleEmitterObject(Texture2D texture, int count, glm::vec3 position = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f),
						  glm::vec3 velocity = glm::vec3(0.0f));

	// Destructor
	~ConeParticleEmitterObject() override;

	// Updates the state
	void Update(GLfloat dt) override;

protected:
	void respawnParticle(Particle& particle) override;
};
