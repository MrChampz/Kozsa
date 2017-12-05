#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include "Texture2D.h"
#include "Shader.h"

// Represents a vertex
struct Vertex
{
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// Tangent
	glm::vec3 Tangent;
	// BiTangent
	glm::vec3 BiTangent;
	// TexCoords
	glm::vec2 TexCoords;
};

struct BoneData
{
	// BoneIndices1
	glm::vec4 BoneIndices1;
	// BoneIndices2
	glm::vec4 BoneIndices2;
	// BoneWeights1
	glm::vec4 BoneWeights1;
	// BoneWeights2
	glm::vec4 BoneWeights2;
};

/**
 * Mesh is able to store, configure and render a mesh in OpenGL.
 * One or more mesh(s) form a model.
 * It also hosts utility functions for easy management.
 */
class Mesh
{
public:
	// Render Data
	GLuint mVAO, mVBO, mEBO;

	// Mesh Data
	std::vector<Vertex>			mVertices;
	std::vector<GLuint>			mIndices;
	std::vector<BoneData>		mBoneData;
	std::vector<Texture2D>		mTextures;

public:
	// EmptyConstructor
	Mesh();

	// Constructor
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture2D> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture2D> textures, std::vector<BoneData> boneData);

	// Destructor
	~Mesh();

	// Render the mesh
	void Draw(Shader& shader);

private:
	// Initializes all the buffer objects/arrays
	void setupMesh();
	void shutdownMesh();
};
