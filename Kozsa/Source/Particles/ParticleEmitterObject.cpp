#include "ParticleEmitterObject.h"

GLfloat particleVertices[] = {
	// Position			// TexCoords
	0.0f, 1.0f,	0.0f,	0.0f, 1.0f,
	1.0f, 0.0f,	0.0f,	1.0f, 0.0f,
	0.0f, 0.0f,	0.0f,	0.0f, 0.0f,

	0.0f, 1.0f,	0.0f,	0.0f, 1.0f,
	1.0f, 1.0f,	0.0f,	1.0f, 1.0f,
	1.0f, 0.0f,	0.0f,	1.0f, 0.0f
};

ParticleEmitterObject::ParticleEmitterObject(Texture2D texture, int count, glm::vec3 position,glm::vec3 size, glm::vec3 velocity)
	: GameObject(position, size, velocity), mTexture(texture), mParticlesCount(count), mNewParticlesCount(2), mParticlesOffset(0.0f, 0.0f, 0.0f),
	  mParticlesSize(0.5f), mParticlesVelocity(0.0f, -10.0f, 0.0f)
{
	this->setupMesh();
	this->setupPhysics();
}

ParticleEmitterObject::~ParticleEmitterObject()
{
	// Delete the VBO and VAO
	glDeleteBuffers(1, &this->mVBO);
	glDeleteVertexArrays(1, &this->mVAO);
}


void ParticleEmitterObject::Update(GLfloat dt)
{
	PxParticleReadData* rd = ps->lockParticleReadData();
	if (rd)
	{
		PxStrideIterator<const PxParticleFlags> flagsIt(rd->flagsBuffer);
		PxStrideIterator<const PxVec3> positionsIt(rd->positionBuffer);

		for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt, ++positionsIt)
		{
			if (*flagsIt & PxParticleFlag::eVALID)
			{
				const PxVec3& position = *positionsIt;

				//mParticles[i].Position = glm::vec3(position.x, position.y, position.z);
			}
		}
	}
	rd->unlock();


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
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 0.2;
		}
	}
}

void ParticleEmitterObject::Draw(Renderer& renderer)
{
	// Use additive blending to give it a "glow" effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	// Render the particles
	for (Particle particle : this->mParticles)
	{
		if (particle.Life > 0.0f)
		{
			// Send the final position, size and color to shader
			renderer.mShader.SetVector3f("offset", (this->mPosition + particle.Position));
			renderer.mShader.SetVector3f("scale", particle.Size);
			renderer.mShader.SetVector4f("color", particle.Color);
			
			renderer.mShader.SetInteger("tex", 0);
			glActiveTexture(GL_TEXTURE0);
			this->mTexture.Bind();

			glBindVertexArray(this->mVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			this->mTexture.Unbind();
		}
	}

	// Reset to default blending mode
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleEmitterObject::setupMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->mVAO);
	glGenBuffers(1, &this->mVBO);

	// Bind VAO
	glBindVertexArray(this->mVAO);

	// Load vertex data into vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleVertices), &particleVertices, GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	// Vertex Texture Coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	// Unbind VAO
	glBindVertexArray(0);

	// Create the specified amount of particle instances
	for (int i = 0; i < this->mParticlesCount; ++i)
	{
		Particle particle;
		float theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		float phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));
		glm::vec3 v(0.0f);

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		float velocity = glm::mix(1.25f, 1.5f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		particle.Position = glm::vec3(10.0f, 10.0f, 10.0f);
		particle.Velocity = v;
		particle.Color = glm::vec4(1.0f);
		particle.Size = glm::vec3(this->mParticlesSize);
		particle.Weight = 1.0f;
		particle.Life = 1.0f;

		this->mParticles.push_back(particle);
	}
}

void ParticleEmitterObject::setupPhysics()
{
	const PxU32 pCount = this->mParticlesCount;
	ps = Physics::mPhysicsSDK->createParticleSystem(pCount);
	ps->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	//ps->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	if (ps)
		Physics::mScene->addActor(*ps);

	PxParticleCreationData particleCreationData;
	particleCreationData.numParticles = pCount;

	PxU32 indexBuffer[1000];
	PxVec3 posBuffer[1000];
	PxVec3 velBuffer[1000];

	for (int i = 0; i < pCount; i++)
	{
		indexBuffer[i] = i;
		posBuffer[i] = PxVec3(mParticles[i].Position.x, mParticles[i].Position.y, mParticles[i].Position.z);
		velBuffer[i] = PxVec3(mParticles[i].Velocity.x, mParticles[i].Velocity.y, mParticles[i].Velocity.z);
	}

	particleCreationData.indexBuffer = PxStrideIterator<const PxU32>(indexBuffer);
	particleCreationData.positionBuffer = PxStrideIterator<const PxVec3>(posBuffer);
	particleCreationData.velocityBuffer = PxStrideIterator<const PxVec3>(velBuffer);

	bool success = ps->createParticles(particleCreationData);
	if (!success)
		printf("ERROR::PARTICLES: Error creating particles!\n");
}

int lastUsedParticle = 0;
int ParticleEmitterObject::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (int i = lastUsedParticle; i < this->mParticlesCount; ++i) {
		if (this->mParticles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	// Otherwise, do a linear search
	for (int i = 0; i < lastUsedParticle; ++i) {
		if (this->mParticles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;

	return 0;
}

void ParticleEmitterObject::respawnParticle(Particle& particle)
{
	float theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
	float phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));
	glm::vec3 v(0.0f);

	v.x = sinf(theta) * cosf(phi);
	v.y = cosf(theta);
	v.z = sinf(theta) * sinf(phi);

	float velocity = glm::mix(1.25f, 1.5f, ((float)rand() / RAND_MAX));
	v = glm::normalize(v) * velocity;

	particle.Position = glm::vec3(10.0f, 10.0f, 10.0f);
	particle.Velocity = (this->mVelocity + v) / particle.Weight;
	particle.Color = glm::vec4(1.0f);
	particle.Size = glm::vec3(this->mParticlesSize);
	particle.Weight = 1.0f;
	particle.Life = 1.0f;
}
