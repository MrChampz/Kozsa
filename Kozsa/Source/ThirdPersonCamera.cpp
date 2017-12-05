#include "ThirdPersonCamera.h"

ThirdPersonCamera::ThirdPersonCamera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
	: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mMovementSpeed(THIRD_CAM_SPEED), mMouseSensitivity(THIRD_CAM_SENSITIVITY), mZoom(THIRD_CAM_ZOOM)
{
	this->mPosition = position;
	this->mWorldUp = up;
	this->mYaw = yaw;
	this->mPitch = pitch;
	this->updateCameraVectors();
}

ThirdPersonCamera::ThirdPersonCamera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ,
	GLfloat yaw, GLfloat pitch)
	: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mMovementSpeed(THIRD_CAM_SPEED), mMouseSensitivity(THIRD_CAM_SENSITIVITY), mZoom(THIRD_CAM_ZOOM)
{
	this->mPosition = glm::vec3(posX, posY, posZ);
	this->mWorldUp = glm::vec3(upX, upY, upZ);
	this->mYaw = yaw;
	this->mPitch = pitch;
	this->updateCameraVectors();
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Update(GLfloat dt)
{
	this->updateCameraVectors();
}

void ThirdPersonCamera::ProcessMouseMovement(CameraRotation rotation, GLfloat offset, GLboolean constrainPitch)
{
	if (rotation == ROT_PITCH)
	{
		float pitchChange = offset * 0.1f;
		this->mPitch -= pitchChange;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->mPitch > -5.0f)
				this->mPitch = -5.0f;
			if (this->mPitch < -89.0f)
				this->mPitch = -89.0f;
		}
	}
	if (rotation == ROT_YAW)
	{
		float angleChange = offset * 0.3f;
		this->mAngleAroundPlayer -= angleChange;
	}
}

void ThirdPersonCamera::ProcessMouseScroll(GLfloat yoffset)
{
	float zoomLevel = yoffset * 0.1f;
	if (this->mDistanceFromPlayer - zoomLevel > 1.0f && this->mDistanceFromPlayer - zoomLevel < 10.0f)
	{
		this->mDistanceFromPlayer -= zoomLevel;
	}
}

glm::mat4 ThirdPersonCamera::GetViewMatrix() const
{
	return glm::lookAt(this->mPosition, this->mPosition + this->mFront, this->mUp);
}

void ThirdPersonCamera::updateCameraVectors()
{
	float horizontalDist = this->mDistanceFromPlayer * cos(glm::radians(this->mPitch));
	float verticalDist = this->mDistanceFromPlayer * sin(glm::radians(this->mPitch));

	float theta = this->mPlayerRotation.y + this->mAngleAroundPlayer;
	float offsetX = horizontalDist * sin(glm::radians(theta));
	float offsetZ = horizontalDist * cos(glm::radians(theta));
	this->mPosition.x = this->mPlayerPosition.x - offsetX;
	this->mPosition.z = this->mPlayerPosition.z - offsetZ;
	this->mPosition.y = this->mPlayerPosition.y - verticalDist;
	this->mYaw = 90.0f - (this->mPlayerRotation.y + this->mAngleAroundPlayer);

	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(this->mYaw)) * cos(glm::radians(this->mPitch));
	front.y = sin(glm::radians(this->mPitch));
	front.z = sin(glm::radians(this->mYaw)) * cos(glm::radians(this->mPitch));
	this->mFront = glm::normalize(front);

	// Also re-calculate the Right and Up vector
	this->mRight = glm::normalize(glm::cross(this->mFront, this->mWorldUp));
	this->mUp = glm::normalize(glm::cross(this->mRight, this->mFront));
}