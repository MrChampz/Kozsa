#include "TestParticleEmitterObject.h"

#include <GLFW\glfw3.h>

TestParticleEmitterObject::TestParticleEmitterObject(Texture2D texture, int count, glm::vec3 position, glm::vec3 size, glm::vec3 velocity)
	: GameObject(position, size, velocity), mTexture(texture), mParticlesCount(count), mNewParticlesCount(2), mParticlesOffset(0.0f, 0.0f, 0.0f),
	mParticlesSize(0.5f), mParticlesVelocity(0.0f, -10.0f, 0.0f), drawBuf(1), deltaT(0.0f), time(0.0f)
{
	nrParticles = 4000;

	this->setupMesh();
}

TestParticleEmitterObject::~TestParticleEmitterObject()
{
	glDeleteBuffers(1, &this->mInitVelVBO);
	glDeleteBuffers(2, this->mStartTimeVBO);
	glDeleteBuffers(2, this->mVelVBO);
	glDeleteBuffers(2, this->mPosVBO);
	glDeleteVertexArrays(2, this->mVAO);
	glDeleteTransformFeedbacks(2, this->mTF);
}

void TestParticleEmitterObject::Update(GLfloat dt)
{
	this->deltaT = dt;
	this->time = glfwGetTime();
}

void TestParticleEmitterObject::Draw(Shader& shaderUpdate, Shader& shaderRender)
{
	const char* outputNames[] = { "Position", "Velocity", "StartTime" };
	glTransformFeedbackVaryings(shaderUpdate.mID, 3, outputNames, GL_SEPARATE_ATTRIBS);
	glLinkProgram(shaderUpdate.mID);

	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &updateSub);

	// Render the particles
	// Send the final position, size and color to shader
	shaderUpdate.SetFloat("ParticleLifetime", 3.5f);
	shaderUpdate.SetVector3f("Accel", glm::vec3(0.0f, -0.4f, 0.0f));


	shaderUpdate.SetFloat("Time", this->time);
	shaderUpdate.SetFloat("H", this->deltaT);

	// Use additive blending to give it a "glow" effect
	glPointSize(10.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->mTF[drawBuf]);

	glBeginTransformFeedback(GL_POINTS);
	glBindVertexArray(this->mVAO[1 - drawBuf]);
	glDrawArrays(GL_POINTS, 0, nrParticles);
	glEndTransformFeedback();

	glDisable(GL_RASTERIZER_DISCARD);
	
	shaderRender.SetFloat("Time", this->time);
	shaderRender.SetFloat("ParticleLifetime", 3.5f);

	shaderRender.SetInteger("texture", 0);
	glActiveTexture(GL_TEXTURE0);
	this->mTexture.Bind();

	glBindVertexArray(this->mVAO[drawBuf]);
	glDrawTransformFeedback(GL_POINTS, mTF[drawBuf]);

	this->mTexture.Unbind();

	// Reset to default blending mode
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	drawBuf = 1 - drawBuf;
}

void TestParticleEmitterObject::setupMesh()
{
	glGenBuffers(2, mPosVBO);
	glGenBuffers(2, mVelVBO);
	glGenBuffers(2, mStartTimeVBO);
	glGenBuffers(1, &mInitVelVBO);

	int size = nrParticles * 3 * sizeof(GLfloat);

	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, mVelVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, mVelVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, mStartTimeVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, nrParticles * sizeof(GLfloat), NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, mStartTimeVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, nrParticles * sizeof(GLfloat), NULL, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, mInitVelVBO);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

	// First Pos
	GLfloat* data = new GLfloat[nrParticles * 3];
	for (int i = 0; i < nrParticles * 3; i++)
		data[i] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fisrt Vels
	float velocity, theta, phi;
	glm::vec3 v(0.0f);

	double pi = 3.14159265359;

	for (int i = 0; i < nrParticles; i++)
	{
		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(1.25f, 1.5f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i + 0] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, mStartTimeVBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nrParticles * sizeof(GLfloat), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(2, mVAO);

	glBindVertexArray(mVAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mVelVBO[0]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mStartTimeVBO[0]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mInitVelVBO);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(mVAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, mPosVBO[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mVelVBO[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mStartTimeVBO[1]);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mInitVelVBO);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);

	glGenTransformFeedbacks(2, this->mTF);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->mTF[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, this->mPosVBO[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, this->mVelVBO[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, this->mStartTimeVBO[0]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->mTF[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, this->mPosVBO[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, this->mVelVBO[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, this->mStartTimeVBO[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}
