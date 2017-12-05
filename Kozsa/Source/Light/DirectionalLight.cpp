#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec4 color)
	: mDirection(direction), mColor(color)
{
}

DirectionalLight::~DirectionalLight()
{
}
