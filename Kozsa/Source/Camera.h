#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system specific input methods
enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const GLfloat YAW			= 90.0f;
const GLfloat PITCH			= 0.0f;
const GLfloat SPEED         = 3.0f;
const GLfloat SENSITIVITY	= 0.25f;
const GLfloat ZOOM			= 45.0f;

/**
 * An abstract camera class that processes input and calculates the
 * corresponding Eular Angles, Vectors and Matrices for use in OpenGL.
 */
class Camera
{
public:
	// Camera Attributes
	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp;

	// Eular Angles
	GLfloat mYaw;
	GLfloat mPitch;

	// Camera Options
	GLfloat mMovementSpeed;
	GLfloat mMouseSensitivity;
	GLfloat mZoom;

public:
	// Construtor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);

	// Constructor with scalars
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch);
	
	//Destructor
	~Camera();

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix() const;

	// Processes input received from any keyboard-like input system.
	// Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	virtual void ProcessKeyboard(CameraMovement direction, GLfloat deltaTime);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction
	virtual void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = GL_TRUE);

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	virtual void ProcessMouseScroll(GLfloat yoffset);

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	virtual void updateCameraVectors();
};
