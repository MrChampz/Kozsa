#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <map>
#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <GLM/gtx/quaternion.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/transform.hpp>

class Skeleton
{
public:
	struct Bone
	{
		Bone(int id, Bone* parent, std::string name, glm::mat4 offsetMatrix)
			: ID(id), Parent(parent), Name(name), OffsetMatrix(offsetMatrix)
		{

		}

		int ID;
		std::string Name;
		glm::mat4 OffsetMatrix;

		Bone* Parent;
		std::vector<Bone*> Children;
	};

	struct Animation
	{
		struct Keyframe
		{
			struct BoneProperty
			{
				int ID;
				glm::vec3 Position;
				glm::quat Rotation;
				glm::vec3 Scale = glm::vec3(1.0f);
			};

			int Index = 0;
			double Time = 0.0;
			std::map<int, Keyframe::BoneProperty> BoneProperties;
		};

		std::string Name;
		double Duration;
		std::vector<Keyframe> Keyframes;
	};

	Bone* mRootBone;
	std::map<int, Bone*> mBones;
	std::map<std::string, Animation> mAnimations;

private:
	std::map<std::string, Bone*> _BonesByName;

public:
	Skeleton();
	~Skeleton();

	int CreateBone(int ID, int parentID, std::string name, glm::mat4 offsetMatrix);

	int GetBoneID(std::string name);

	std::vector<glm::mat4> GetFrameBones(std::string animationName, double time, bool noRootMotion = false);
	std::vector<glm::mat4> Blend(std::string CurrentAnimationName, std::string newAnimationName, double time, bool noRootMotion = false);
	void AccumulateBoneTransforms(bool noRootMotion, Animation::Keyframe& currentFrame, Animation::Keyframe& nextFrame, float progress, std::map<int, glm::mat4>& boneMatrices, Bone* bone, glm::mat4 parentMatrix);
	void PrintSkeleton();
	void PrintSkeleton(Bone* bone, int depthCount);

private:
	int GetKeyframe(Animation& animation, double time);
};

