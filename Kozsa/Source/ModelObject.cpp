#include "ModelObject.h"

ModelObject::ModelObject()
	: GameObject(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec4(1.0f)), mModel()
{
}


ModelObject::ModelObject(const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color)
	: GameObject(position, size, velocity, color), mModel()
{
}

ModelObject::ModelObject(Model* model, const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color)
	: GameObject(position, size, velocity, color), mModel(model)
{
}

ModelObject::~ModelObject()
{
}


void ModelObject::Update(GLfloat dt)
{
}

void ModelObject::Draw(Renderer& renderer)
{
	renderer.DrawModel(this->mModel, this->mPosition, this->mRotation, this->mSize);
}