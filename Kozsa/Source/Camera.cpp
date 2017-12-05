#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
	: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mMovementSpeed(SPEED), mMouseSensitivity(SENSITIVITY), mZoom(ZOOM)
{
	this->mPosition = position;
	this->mWorldUp = up;
	this->mYaw = yaw;
	this->mPitch = pitch;
	this->updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
	: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mMovementSpeed(SPEED), mMouseSensitivity(SENSITIVITY), mZoom(ZOOM)
{
	this->mPosition = glm::vec3(posX, posY, posZ);
	this->mWorldUp = glm::vec3(upX, upY, upZ);
	this->mYaw = yaw;
	this->mPitch = pitch;
	this->updateCameraVectors();
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix() const 
{
	return glm::lookAt(this->mPosition, this->mPosition + this->mFront, this->mUp);
}

void Camera::ProcessKeyboard(CameraMovement direction, GLfloat deltaTime)
{
	GLfloat velocity = this->mMovementSpeed * deltaTime;
	if (direction == FORWARD)
		this->mPosition += this->mFront * velocity;
	if (direction == BACKWARD)
		this->mPosition -= this->mFront * velocity;
	if (direction == LEFT)
		this->mPosition -= this->mRight * velocity;
	if (direction == RIGHT)
		this->mPosition += this->mRight * velocity;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	xoffset *= this->mMouseSensitivity;
	yoffset *= this->mMouseSensitivity;

	this->mYaw		+= xoffset;
	this->mPitch	+= yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if(constrainPitch)
	{
		if (this->mPitch > 89.0f)
			this->mPitch = 89.0f;
		if (this->mPitch < -89.0f)
			this->mPitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	this->updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset)
{
	if (this->mZoom >= 1.0f && this->mZoom <= 45.0f)
		this->mZoom -= yoffset;
	if (this->mZoom <= 1.0f)
		this->mZoom = 1.0f;
	if (this->mZoom >= 45.0f)
		this->mZoom = 45.0f;

}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(this->mYaw)) * cos(glm::radians(this->mPitch));
	front.y = sin(glm::radians(this->mPitch));
	front.z = sin(glm::radians(this->mYaw)) * cos(glm::radians(this->mPitch));
	this->mFront = glm::normalize(front);

	// Also re-calculate the Right and Up vector
	this->mRight	= glm::normalize(glm::cross(this->mFront, this->mWorldUp));
	this->mUp		= glm::normalize(glm::cross(this->mRight, this->mFront));
}
