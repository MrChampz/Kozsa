#pragma once

#include <GLM/glm.hpp>

class PointLight
{
public:
	// State
	glm::vec3 mPosition;
	glm::vec4 mColor;

public:
	// Constructor
	PointLight(glm::vec3 position = glm::vec3(0.0f), glm::vec4 color =  glm::vec4(1.0f));

	// Destructor
	~PointLight();
};