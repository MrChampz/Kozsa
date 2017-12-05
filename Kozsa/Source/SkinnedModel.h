#pragma once

#include <string>
#include <vector>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace Assimp;

#include "Mesh.h"
#include "AnimatedMesh.h"
#include "Skeleton.h"
#include "Shader.h"

/**
 * Model is able to store, configure and render a model in OpenGL.
 * A Model contains one or more mesh(s), that are rendered individually.
 * It also hosts utility functions for easy management.
 */
class SkinnedModel
{
public:
	// Model Data
	std::string mDirectory;
	std::vector<Mesh> mMeshes;
	Skeleton* mSkeleton;

	std::string mAnimationName;
	double mAnimationTime = 0;

private:
	double mSpeed = 1.0;
	bool mNoRootMotion = true;
	bool mIsPlaying = false;
	std::string mCurrentAnimation;
	double _AnimationTime = 0.0f;

public:
	// Empty Constructor
	SkinnedModel(std::string filepath);
	
	// Constructor
	SkinnedModel(std::vector<Mesh> meshes, Skeleton* skeleton, std::string directory);

	// Destructor
	~SkinnedModel();

	// Draws the model, and thus all its meshes
	void Draw(Shader& shader);

	double Speed() const { return mSpeed; }
	void SetSpeed(double animationSpeed) { mSpeed = animationSpeed; }
	std::string Animation() const { return mCurrentAnimation; };
	void SetAnimation(std::string animationName) { mCurrentAnimation = animationName; }
	bool NoRootMotion() const { return mNoRootMotion; }
	void SetNoRootMotion(bool noRootMotion) { mNoRootMotion = noRootMotion; }
	void Play() { mIsPlaying = true; }
	void Pause() { mIsPlaying = false; }
	bool Paused() { return !mIsPlaying; }
	void Stop() { mIsPlaying = false; _AnimationTime = 0; }

	void Update(double dt)
	{
		if (mIsPlaying)
		{
			_AnimationTime += mSpeed * dt;
		}
	}
	
private:
	void loadModel(std::string filepath);
	void processNode(aiNode* node, const aiScene* scene);
	AnimatedMesh processMesh(aiMesh* mesh, const aiScene* scene);
	void processSkeleton(const aiScene* scene);
	void processAnimations(const aiScene* scene);
	void createSkeleton(std::vector<std::tuple<std::string, glm::mat4>>& boneInfo, std::map<std::string, int>& boneNameMapping, aiNode* node, int parentID);
};
