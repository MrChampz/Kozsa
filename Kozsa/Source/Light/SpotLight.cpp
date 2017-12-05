#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec4 color)
	: mPosition(position), mDirection(direction), mColor(color)
{
}

SpotLight::~SpotLight()
{
}
