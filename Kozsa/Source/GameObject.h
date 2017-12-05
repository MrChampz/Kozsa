#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include "Renderer.h"

/**
 * Container object for holding all state relevant for a single
 * game object entity. Each object in the game likely needs the
 * minimal of state as described within GameObject.
 */
class GameObject
{
public:
	// GameObject State
	glm::vec3 mPosition, mSize, mVelocity;
	glm::vec4 mColor;
	glm::quat mRotation;
	GLboolean mDestroyed;

public:
	// Constructor/Destructor
	GameObject();
	GameObject(const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity = glm::vec3(0.0f), 
		const glm::vec4 color = glm::vec4(1.0f));
	virtual ~GameObject();

	// Updates the object state
	virtual void Update(GLfloat dt);

	// Draws the object
	virtual void Draw(Renderer& renderer);
};