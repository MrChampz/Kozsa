#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec4 color)
	: mPosition(position), mColor(color)
{
}

PointLight::~PointLight()
{
}
