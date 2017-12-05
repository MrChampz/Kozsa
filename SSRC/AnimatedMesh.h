#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include "Texture2D.h"
#include "Shader.h"

// Represents a vertex
struct AnimatedVertex
{
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;

	int IDs1[4];
	int IDs2[4];
	float Weights1[4];
	float Weights2[4];
};

/**
 * Mesh is able to store, configure and render a mesh in OpenGL.
 * One or more mesh(s) form a model.
 * It also hosts utility functions for easy management.
 */
class AnimatedMesh
{
public:
	// Render Data
	GLuint mVAO, mVBO, mEBO;

	// Mesh Data
	std::vector<AnimatedVertex>			mVertices;
	std::vector<GLuint>			mIndices;
	std::vector<Texture2D>		mTextures;

public:
	// EmptyConstructor
	AnimatedMesh();

	// Constructor
	AnimatedMesh(std::vector<AnimatedVertex> vertices, std::vector<GLuint> indices, std::vector<Texture2D> textures);

	// Destructor
	~AnimatedMesh();

	// Render the mesh
	void Draw(Shader& shader);

private:
	// Initializes all the buffer objects/arrays
	void setupMesh();
	void shutdownMesh();
};
