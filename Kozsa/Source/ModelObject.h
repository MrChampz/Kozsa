#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include "GameObject.h"
#include "Renderer.h"
#include "Model.h"

class ModelObject : public GameObject
{
public:
	// ModelObject State
	Model* mModel;

public:
	// Constructor
	ModelObject();
	ModelObject(const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color);
	ModelObject(Model* model, const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color);
	// Destructor
	virtual __thiscall ~ModelObject() override;

	// Updates the model object state
	virtual void Update(GLfloat dt) override;

	// Draws the model object
	virtual void Draw(Renderer& renderer) override;
};
