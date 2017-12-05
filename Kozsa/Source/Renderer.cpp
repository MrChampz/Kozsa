#include "Renderer.h"

#include <GLM/gtx/quaternion.hpp>

Renderer::Renderer()
{
}

Renderer::Renderer(Shader& shader)
{
	this->mShader = shader;
}

Renderer::~Renderer()
{
	
}


void Renderer::DrawModel(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 size)
{
	// Prepare transformations
	this->mShader.Use();
	glm::mat4 modelMatrix;

	// Translate, rotate and scale
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, size);

	// Input modelMatrix into shader
	this->mShader.SetMatrix4("model", modelMatrix);

	// Render model
	model->Draw(this->mShader);
}

void Renderer::DrawModel(Model* model, glm::vec3 position, glm::quat rotation, glm::vec3 size)
{
	// Prepare transformations
	this->mShader.Use();
	glm::mat4 modelMatrix;

	// Translate, rotate and scale
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix *= glm::mat4_cast(rotation);
	modelMatrix = glm::scale(modelMatrix, size);

	// Input modelMatrix into shader
	this->mShader.SetMatrix4("model", modelMatrix);

	// Render model
	model->Draw(this->mShader);
}
