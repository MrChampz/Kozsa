#include "Frustum.h"

Frustum::Frustum(float screenDepth)
	:	mScreenDepth(screenDepth)
{
}

Frustum::~Frustum()
{
}

void Frustum::ConstructFrustum(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	float zMinimum, r;
	
	glm::mat4 matrix;
	matrix = projectionMatrix;

	// Calculate the minimum Z distance in the frustum
	zMinimum = -matrix[3][2] / -matrix[2][2];
	r = this->mScreenDepth / (this->mScreenDepth - zMinimum);

	// Load the updated values back into the projection matrix
	matrix[2][2] = -r;
	matrix[3][2] = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix
	matrix *= viewMatrix;
	matrix = glm::transpose(matrix);

	// Calculate near plane of frustum
	this->mPlanes[0].x = matrix[3][0] + matrix[2][0];
	this->mPlanes[0].y = matrix[3][1] + matrix[2][1];
	this->mPlanes[0].z = matrix[3][2] + matrix[2][2];
	this->mPlanes[0].w = matrix[3][3] + matrix[2][3];

	// Calculate far plane of frustum
	this->mPlanes[1].x = matrix[3][0] - matrix[2][0];
	this->mPlanes[1].y = matrix[3][1] - matrix[2][1];
	this->mPlanes[1].z = matrix[3][2] - matrix[2][2];
	this->mPlanes[1].w = matrix[3][3] - matrix[2][3];

	// Calculate left plane of frustum
	this->mPlanes[2].x = matrix[3][0] + matrix[0][0];
	this->mPlanes[2].y = matrix[3][1] + matrix[0][1];
	this->mPlanes[2].z = matrix[3][2] + matrix[0][2];
	this->mPlanes[2].w = matrix[3][3] + matrix[0][3];

	// Calculate right plane frustum
	this->mPlanes[3].x = matrix[3][0] - matrix[0][0];
	this->mPlanes[3].y = matrix[3][1] - matrix[0][1];
	this->mPlanes[3].z = matrix[3][2] - matrix[0][2];
	this->mPlanes[3].w = matrix[3][3] - matrix[0][3];

	// Calculate top plane of frustum
	this->mPlanes[4].x = matrix[3][0] - matrix[1][0];
	this->mPlanes[4].y = matrix[3][1] - matrix[1][1];
	this->mPlanes[4].z = matrix[3][2] - matrix[1][2];
	this->mPlanes[4].w = matrix[3][3] - matrix[1][3];

	// Calculate bottom plane frustum
	this->mPlanes[5].x = matrix[3][0] + matrix[1][0];
	this->mPlanes[5].y = matrix[3][1] + matrix[1][1];
	this->mPlanes[5].z = matrix[3][2] + matrix[1][2];
	this->mPlanes[5].w = matrix[3][3] + matrix[1][3];

	// Normalize the planes
	float length;
	for (int i = 0; i < 6; i++)
	{
		length = sqrtf(
			(this->mPlanes[i].x * this->mPlanes[i].x) +
			(this->mPlanes[i].y * this->mPlanes[i].y) +
			(this->mPlanes[i].z * this->mPlanes[i].z));
		this->mPlanes[i].x /= length;
		this->mPlanes[i].y /= length;
		this->mPlanes[i].z /= length;
		this->mPlanes[i].w /= length;
	}
}

bool Frustum::CheckRectangle2(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth)
{
	float dotProduct;

	// Check if any of the 6 planes of the rectangle are inside the view frustum
	for (int i = 0; i < 6; i++)
	{
		dotProduct = ((this->mPlanes[i].x * minWidth) + (this->mPlanes[i].y * minHeight) +
					  (this->mPlanes[i].z * minDepth) + (this->mPlanes[i].w * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((this->mPlanes[i].x * maxWidth) + (this->mPlanes[i].y * minHeight) +
					  (this->mPlanes[i].z * minDepth) + (this->mPlanes[i].w * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((this->mPlanes[i].x * minWidth) + (this->mPlanes[i].y * maxHeight) +
					  (this->mPlanes[i].z * minDepth) + (this->mPlanes[i].w * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((this->mPlanes[i].x * maxWidth) + (this->mPlanes[i].y * maxHeight) +
					  (this->mPlanes[i].z * minDepth) + (this->mPlanes[i].w * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((this->mPlanes[i].x * minWidth) + (this->mPlanes[i].y * minHeight) +
					  (this->mPlanes[i].z * maxDepth) + (this->mPlanes[i].w * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((this->mPlanes[i].x * maxWidth) + (this->mPlanes[i].y * minHeight) +
					  (this->mPlanes[i].z * maxDepth) + (this->mPlanes[i].w * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((this->mPlanes[i].x * minWidth) + (this->mPlanes[i].y * maxHeight) +
					  (this->mPlanes[i].z * maxDepth) + (this->mPlanes[i].w * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((this->mPlanes[i].x * maxWidth) + (this->mPlanes[i].y * maxHeight) +
					  (this->mPlanes[i].z * maxDepth) + (this->mPlanes[i].w * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}


		return false;
	}

	return true;
}
