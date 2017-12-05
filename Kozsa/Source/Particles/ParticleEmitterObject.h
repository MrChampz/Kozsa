#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/quaternion.hpp>

#include "../GameObject.h"
#include "../Renderer.h"
#include "../Texture2D.h"

// Represents a single particle and its state
struct Particle
{
	glm::vec3 Position;
	glm::vec3 Size;
	glm::vec3 Velocity;
	glm::vec4 Color;
	GLfloat Weight;
	GLfloat Life;

	Particle()
		  : Position(0.0f, 0.0f, 0.0f), Size(0.5f), Velocity(0.0f, 10.0f, 0.0f), Color(1.0f), Weight(0.0f), Life(0.0f) {}
};

class ParticleEmitterObject : public GameObject
{
public:
	// Render Data
	GLuint mVAO, mVBO;
	Texture2D mTexture;

	// State
	std::vector<Particle> mParticles;
	glm::vec3 mParticlesOffset;
	glm::vec3 mParticlesSize;
	glm::vec3 mParticlesVelocity;
	int mParticlesCount;
	int mNewParticlesCount;

	// Physics State
	PxParticleSystem* ps;

public:
	// Constructor
	ParticleEmitterObject(Texture2D texture, int count, glm::vec3 position = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f),
						  glm::vec3 velocity = glm::vec3(0.0f));

	// Destructor
	virtual ~ParticleEmitterObject() override;

	// Updates the state
	virtual void Update(GLfloat dt) override;

	// Draws the particle emitter object
	virtual void Draw(Renderer& renderer) override;

protected:
	void setupMesh();
	void setupPhysics();
	int firstUnusedParticle();
	virtual void respawnParticle(Particle& particle);
};
