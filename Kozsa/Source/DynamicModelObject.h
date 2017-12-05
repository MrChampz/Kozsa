#pragma once

#include "ModelObject.h"
#include "Physics/Physics.h"

class DynamicModelObject : public ModelObject
{
public:
	// State
	PxRigidDynamic* mRigidBody;

public:
	// Constructor
	DynamicModelObject(const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color);
	DynamicModelObject(Model* model, const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color);
	// Destructor
	~DynamicModelObject() override;

	// Updates the dynamic model object state
	void Update(GLfloat dt) override;

	// Draws the dynamic  model object
	void Draw(Renderer& renderer) override;

private:
	void setupRigidBody();
};