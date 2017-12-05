#pragma once

#include <GL/glew.h>
#include  <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Shader.h"

class Renderer
{
public:
	// Render State
	Shader mShader;

public:
	// Constructor/Destructor
	Renderer();
	Renderer(Shader& shader);
	~Renderer();

	// Renders a model
	void DrawModel(Model* model, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 size = glm::vec3(1.0f));
	void DrawModel(Model* model, glm::vec3 position, glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f),
				   glm::vec3 size = glm::vec3(1.0f));
};