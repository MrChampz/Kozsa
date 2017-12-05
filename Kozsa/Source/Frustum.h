#pragma once

#include <GLM/glm.hpp>

class Frustum
{
public:
	// State
	float mScreenDepth;
	glm::vec4 mPlanes[6];

public:
	// Constructor
	Frustum(float screenDepth = 1000.0f);

	// Destructor
	~Frustum();

	void ConstructFrustum(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

	bool CheckRectangle2(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);
};