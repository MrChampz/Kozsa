#pragma once

#include "ModelObject.h"
#include "Physics/Physics.h"

class StaticModelObject : public ModelObject
{
public:
	// State
	PxRigidStatic* mRigidBody;

public:
	// Constructor
	StaticModelObject(const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color);
	StaticModelObject(Model* model, const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color);
	// Destructor
	~StaticModelObject() override;

	// Updates the static model object state
	void Update(GLfloat dt) override;

	// Draws the static  model object
	void Draw(Renderer& renderer) override;

private:
	void setupRigidBody();
};