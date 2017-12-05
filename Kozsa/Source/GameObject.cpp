#include "GameObject.h"

GameObject::GameObject()
	: mPosition(0.0f), mRotation(0.0f, 0.0f, 0.0f, 0.0f), mSize(1.0f), mVelocity(0.0f), mColor(1.0f), mDestroyed(GL_FALSE)
{
}

GameObject::GameObject(glm::vec3 position, glm::vec3 size, glm::vec3 velocity, glm::vec4 color)
	: mPosition(position), mRotation(0.0f, 0.0f, 0.0f, 0.0f), mSize(size), mVelocity(velocity), mColor(color), mDestroyed(GL_FALSE)
{
}

GameObject::~GameObject()
{
}

void GameObject::Update(GLfloat dt)
{
}

void GameObject::Draw(Renderer& renderer)
{
}
