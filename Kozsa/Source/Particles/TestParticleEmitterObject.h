#pragma once

#include "../GameObject.h"

// Represents a single particle and its state
struct PParticle
{
	glm::vec3 Position;
	glm::vec3 Size;
	glm::vec3 Velocity;
	glm::vec4 Color;
	GLfloat Weight;
	GLfloat Life;
	GLfloat StartTime;

	PParticle()
		: Position(0.0f, 0.0f, 0.0f), Size(0.5f), Velocity(0.0f, 10.0f, 0.0f), Color(1.0f), Weight(0.0f), Life(0.0f), StartTime(0.0) {}
};

class TestParticleEmitterObject : public GameObject
{
public:
	// Render Data
	GLuint mTF[2];
	GLuint mVAO[2];
	GLuint mPosVBO[2];
	GLuint mVelVBO[2];
	GLuint mStartTimeVBO[2];
	GLuint mInitVelVBO;
	GLuint drawBuf, query;
	GLuint renderSub, updateSub;
	int nrParticles;

	Texture2D mTexture;

	// State
	std::vector<PParticle> mParticles;
	glm::vec3 mParticlesOffset;
	glm::vec3 mParticlesSize;
	glm::vec3 mParticlesVelocity;
	int mParticlesCount;
	int mNewParticlesCount;

	float deltaT, time;

public:
	// Constructor
	TestParticleEmitterObject(Texture2D texture, int count, glm::vec3 position = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f),
		glm::vec3 velocity = glm::vec3(0.0f));

	// Destructor
	virtual ~TestParticleEmitterObject() override;

	// Updates the state
	virtual void Update(GLfloat dt) override;

	// Draws the particle emitter object
	virtual void Draw(Shader& shaderUpdate, Shader& shaderRender);

protected:
	void setupMesh();
};