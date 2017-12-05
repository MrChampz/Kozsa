#pragma once

#include <string>
#include <vector>

#include "Mesh.h"
#include "Shader.h"

/**
 * Model is able to store, configure and render a model in OpenGL.
 * A Model contains one or more mesh(s), that are rendered individually.
 * It also hosts utility functions for easy management.
 */
class Model
{
public:
	// Model Data
	std::string mDirectory;
	Mesh* mMeshes;
	int mMeshCount;

public:
	// Empty Constructor
	Model();
	
	// Constructor
	Model(Mesh* meshes, int meshCount, std::string directory);

	// Destructor
	~Model();

	// Draws the model, and thus all its meshes
	void Draw(Shader& shader);
};
