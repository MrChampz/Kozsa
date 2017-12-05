#pragma once

#include <GL\glew.h>
#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>

// Defines several possible options for camera rotation.
// Used as abstraction to stay away from window-system specific input methods
enum CameraRotation
{
	ROT_YAW,
	ROT_PITCH
};

// Default camera values
const float THIRD_CAM_YAW = 90.0f;
const float THIRD_CAM_PITCH = -30.0f;
const float THIRD_CAM_SPEED = 3.0f;
const float THIRD_CAM_SENSITIVITY = 0.25f;
const float THIRD_CAM_ZOOM = 45.0f;

class ThirdPersonCamera
{
public:
	float mDistanceFromPlayer = 5.0f;
	float mAngleAroundPlayer = 0.0f;

	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp;

	glm::vec3 mPlayerPosition;
	glm::vec3 mPlayerRotation;

	GLfloat mYaw;
	GLfloat mPitch;

	GLfloat mMovementSpeed;
	GLfloat mMouseSensitivity;
	GLfloat mZoom;

public:
	// Construtor with vectors
	ThirdPersonCamera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = THIRD_CAM_YAW, GLfloat pitch = THIRD_CAM_PITCH);
	
	// Construtor with scalars
	ThirdPersonCamera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch);

	// Destructor
	~ThirdPersonCamera();

	void Update(GLfloat dt);

	// Processes input received from any keyboard-like input system.
	// Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	//void ProcessKeyboard(CameraMovement direction, GLfloat deltaTime);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction
	void ProcessMouseMovement(CameraRotation rotation, GLfloat offset, GLboolean constrainPitch = GL_TRUE);

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset);

	glm::mat4 GetViewMatrix() const;

private:
	void updateCameraVectors();
};