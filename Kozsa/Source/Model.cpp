#include "Model.h"

#include <iostream>
#include <string>

Model::Model()
	: mMeshes(0), mMeshCount(0)
{
}

Model::Model(Mesh* meshes, int meshCount, std::string directory)
	: mMeshes(0), mMeshCount(0)
{
	// Set the meshes count
	this->mMeshCount = meshCount;

	// Initialize meshes array
	this->mMeshes = new Mesh[this->mMeshCount];

	// Copy meshes data into arrays
	this->mMeshes = meshes;

	// Set the model directory
	this->mDirectory = directory;
}

Model::~Model()
{
	delete[] this->mMeshes;
	this->mMeshes = 0;
}

void Model::Draw(Shader& shader)
{
	// Render each mesh of this model
	for (GLuint i = 0; i < this->mMeshCount; i++)
		this->mMeshes[i].Draw(shader);
}