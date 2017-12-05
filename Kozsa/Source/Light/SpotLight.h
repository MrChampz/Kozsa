#pragma once

#include <GLM/glm.hpp>

class SpotLight
{
public:
	// State
	glm::vec3 mPosition;
	glm::vec3 mDirection;
	glm::vec4 mColor;

public:
	// Constructor
	SpotLight(glm::vec3 position = glm::vec3(0.0f), glm::vec3 direction = glm::vec3(0.0f), glm::vec4 color =  glm::vec4(1.0f));

	// Destructor
	~SpotLight();
};