#pragma once

#include "Physics/Physics.h"
#include "ModelObject.h"
#include "ThirdPersonCamera.h"

enum CharacterMovement
{
	CHAR_FORWARD,
	CHAR_BACKWARD,
	CHAR_LEFT,
	CHAR_RIGHT,
	CHAR_UP,
	CHAR_DOWN
};

class CharacterObject : public ModelObject
{
public:
	// State
	ThirdPersonCamera mCamera;
	PxController* mCharacterController;
	GLfloat mMovementSpeed = 3.0f;

public:
	//Constructors
	CharacterObject();
	CharacterObject(const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color);
	CharacterObject(Model* model, const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color);

	// Destructor
	~CharacterObject();

	// Updates car state
	void Update(GLfloat dt) override;

	// Draws the car
	void Draw(Renderer& renderer) override;

	// Processes input received from any keyboard-like input system.
	// Accepts input parameter in the form of character defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(CharacterMovement direction, GLfloat dt);

private:
	void setupController();
};