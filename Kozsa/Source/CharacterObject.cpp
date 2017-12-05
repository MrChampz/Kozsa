#include "CharacterObject.h"

CharacterObject::CharacterObject()
	: ModelObject(), mCamera()
{
	this->setupController();
}

CharacterObject::CharacterObject(const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color)
	: ModelObject(position, size, velocity, color), mCamera()
{
	this->mCamera.mPlayerPosition = this->mPosition;
	this->mCamera.mPlayerRotation = glm::vec3(0.0f);
	this->setupController();
}

CharacterObject::CharacterObject(Model* model, const glm::vec3 position, const glm::vec3 size, const glm::vec3 velocity, const glm::vec4 color)
	: ModelObject(model, position, size, velocity, color), mCamera()
{
	this->mCamera.mPlayerPosition = this->mPosition;
	this->mCamera.mPlayerRotation = glm::vec3(0.0f);
	this->setupController();
}

CharacterObject::~CharacterObject()
{
	this->mCharacterController->release();
}

void CharacterObject::Update(GLfloat dt)
{
	// Gravity
	GLfloat velocity = this->mMovementSpeed * dt;
	this->mCharacterController->move(Physics::mScene->getGravity() * velocity, 0.0f, dt, nullptr);

	// Update the position
	PxExtendedVec3 pos = this->mCharacterController->getFootPosition();
	this->mPosition = glm::vec3(pos.x, pos.y, pos.z);

	// Update the camera
	this->mCamera.mPlayerPosition = this->mPosition;
	this->mCamera.Update(dt);
}

void CharacterObject::Draw(Renderer& renderer)
{
	renderer.DrawModel(this->mModel, this->mPosition, this->mRotation, this->mSize);
}

void CharacterObject::ProcessKeyboard(CharacterMovement direction, GLfloat dt)
{
	//PxFilterData* fData = new PxFilterData();
	PxControllerFilters filters;
	//filters.mFilterData = fData;
	GLfloat velocity = this->mMovementSpeed * dt;
	if (direction == CHAR_FORWARD)
		//this->mPosition += this->mFront * velocity;
		this->mCharacterController->move(PxVec3(0.0f, 0.0f, 1.0f) * velocity, 0.0f, dt, nullptr);
	if (direction == CHAR_BACKWARD)
		//this->mPosition -= this->mFront * velocity;
		this->mCharacterController->move(PxVec3(0.0f, 0.0f, -1.0f) * velocity, 0.0f, dt, nullptr);
	if (direction == CHAR_LEFT)
		//this->mPosition -= this->mRight * velocity;
		this->mCharacterController->move(PxVec3(1.0f, 0.0f, 0.0f) * velocity, 0.0f, dt, nullptr);
	if (direction == CHAR_RIGHT)
		//this->mPosition += this->mRight * velocity;
		this->mCharacterController->move(PxVec3(-1.0f, 0.0f, 0.0f) * velocity, 0.0f, dt, nullptr);
	if (direction == CHAR_UP)
		//this->mPosition += this->mRight * velocity;
		this->mCharacterController->move(PxVec3(0.0f, 1.0f, 0.0f) * velocity, 0.0f, dt, nullptr);
	if (direction == CHAR_DOWN)
		//this->mPosition += this->mRight * velocity;
		this->mCharacterController->move(PxVec3(0.0f, -1.0f, 0.0f) * velocity, 0.0f, dt, nullptr);
}

void CharacterObject::setupController()
{
	// Character Controller
	PxCapsuleControllerDesc controllerDesc;
	controllerDesc.setToDefault();
	controllerDesc.material = Physics::mMaterial;
	controllerDesc.radius = 0.2f;
	controllerDesc.height = 0.5f;
	controllerDesc.stepOffset = 0.2f;
	controllerDesc.slopeLimit = cosf(glm::radians(45.0f));;
	controllerDesc.upDirection = PxVec3(0.0f, 1.0f, 0.0f);
	controllerDesc.contactOffset = 0.001f;

	this->mCharacterController = Physics::mControllerManager->createController(controllerDesc);
	this->mCharacterController->setFootPosition(PxExtendedVec3(this->mPosition.x, this->mPosition.y, this->mPosition.z));
}