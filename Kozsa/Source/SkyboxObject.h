#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include "GameObject.h"
#include "Renderer.h"
#include "TextureCubemap.h"

class SkyboxObject : public GameObject
{
public:
	// Render Data
	GLuint mVAO, mVBO;
	TextureCubemap mCubemap;

public:
	// Constructor
	SkyboxObject(TextureCubemap cubemap);

	// Destructor
	~SkyboxObject() override;

	// Updates the state
	void Update(GLfloat dt) override;

	// Draws the skybox object
	void Draw(Renderer& renderer) override;

private:
	void setupMesh();
};
