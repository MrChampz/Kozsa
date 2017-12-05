#include "Skeleton.h"

Skeleton::Skeleton()
{
}


Skeleton::~Skeleton()
{
	for (auto &kv : mBones)
	{
		delete kv.second;
	}
}

int Skeleton::CreateBone(int ID, int parentID, std::string name, glm::mat4 offsetMatrix)
{
	if (_BonesByName.find(name) != _BonesByName.end())
	{
		return _BonesByName.at(name)->ID;
	}
	else
	{
		Bone* bone;

		if (parentID == -1)
		{
			bone = new Bone(ID, nullptr, name, offsetMatrix);
			mRootBone = bone;
		}
		else
		{
			Bone* parent = mBones[parentID];
			bone = new Bone(ID, parent, name, offsetMatrix);
			parent->Children.push_back(bone);
		}

		mBones[ID] = bone;
		_BonesByName[name] = bone;

		return ID;
	}
}

int Skeleton::GetBoneID(std::string name)
{
	if (_BonesByName.find(name) == _BonesByName.end())
	{
		return -1;
	}
	else
	{
		return _BonesByName.at(name)->ID;
	}
}

std::vector<glm::mat4> Skeleton::GetFrameBones(std::string animationName, double time, bool noRootMotion)
{
	auto& animation = mAnimations.at(animationName);

	while (time < 0)
		time += animation.Duration;
	while (time > animation.Duration)
		time -= animation.Duration;

	int currentKeyframeIndex = GetKeyframe(animation, time);

	Animation::Keyframe& currentFrame = animation.Keyframes[currentKeyframeIndex];
	Animation::Keyframe& nextFrame = animation.Keyframes[currentKeyframeIndex + 1];
	float alpha = (time - currentFrame.Time) / (nextFrame.Time - currentFrame.Time);

	std::map<int, glm::mat4> frameBones;
	AccumulateBoneTransforms(noRootMotion, currentFrame, nextFrame, alpha, frameBones, mRootBone, glm::mat4(1));

	std::vector<glm::mat4> finalMatrices;
	for (auto& kv : frameBones)
	{
		finalMatrices.push_back(kv.second);
	}

	return finalMatrices;
}

std::vector<glm::mat4> Skeleton::Blend(std::string currentAnimationName, std::string newAnimationName, double time, bool noRootMotion)
{
	auto& currentAnimation = mAnimations.at(currentAnimationName);
	auto& newAnimation = mAnimations.at(newAnimationName);

	/*
	while (time < 0)
		time += currentAnimation.Duration;
	while (time > currentAnimation.Duration)
		time -= currentAnimation.Duration;
	*/

	int currentKeyframeIndex = GetKeyframe(currentAnimation, time);
	int newKeyframeIndex = GetKeyframe(newAnimation, 0.0f);

	Animation::Keyframe& currentFrame = currentAnimation.Keyframes[currentKeyframeIndex];
	Animation::Keyframe& nextFrame = newAnimation.Keyframes[newKeyframeIndex];
	float alpha = (time - currentFrame.Time) / (nextFrame.Time - currentFrame.Time);

	std::map<int, glm::mat4> frameBones;
	AccumulateBoneTransforms(noRootMotion, currentFrame, nextFrame, alpha, frameBones, mRootBone, glm::mat4(1));

	std::vector<glm::mat4> finalMatrices;
	for (auto& kv : frameBones)
	{
		finalMatrices.push_back(kv.second);
	}

	return finalMatrices;
}

void Skeleton::AccumulateBoneTransforms(bool noRootMotion, Animation::Keyframe & currentFrame, Animation::Keyframe & nextFrame, float progress,
	std::map<int, glm::mat4>& boneMatrices, Bone * bone, glm::mat4 parentMatrix)
{
	glm::mat4 boneMatrix;

	if (currentFrame.BoneProperties.find(bone->ID) != currentFrame.BoneProperties.end() || nextFrame.BoneProperties.find(bone->ID) != nextFrame.BoneProperties.end())
	{
		Animation::Keyframe::BoneProperty currentBoneProperty = currentFrame.BoneProperties.at(bone->ID);
		Animation::Keyframe::BoneProperty nextBoneProperty = nextFrame.BoneProperties.at(bone->ID);

		glm::vec3 positionInterp = currentBoneProperty.Position * (1.f - progress) + nextBoneProperty.Position * progress;
		glm::quat rotationInterp = glm::slerp(currentBoneProperty.Rotation, nextBoneProperty.Rotation, progress);
		glm::vec3 scaleInterp = currentBoneProperty.Scale * (1.f - progress) + nextBoneProperty.Scale * progress;

		if (bone == mRootBone && noRootMotion)
		{
			positionInterp.x = 0;
			positionInterp.z = 0;
		}

		boneMatrix = parentMatrix * (glm::translate(positionInterp) * glm::toMat4(rotationInterp) * glm::scale(scaleInterp));
		boneMatrices[bone->ID] = boneMatrix * bone->OffsetMatrix;
	}
	else
	{
		boneMatrix = parentMatrix * bone->Parent->OffsetMatrix;
		boneMatrices[bone->ID] = boneMatrix;
	}

	for (auto& child : bone->Children)
	{
		std::string name = child->Name;
		AccumulateBoneTransforms(noRootMotion, currentFrame, nextFrame, progress, boneMatrices, child, boneMatrix);
	}
}

void Skeleton::PrintSkeleton()
{
	PrintSkeleton(mRootBone, 0);
}

void Skeleton::PrintSkeleton(Bone * bone, int depthCount)
{
	std::stringstream ss;
	ss << std::string(depthCount, ' ');
	ss << bone->ID << ": " << bone->Name;
	std::cout << ss.str() << std::endl;

	depthCount++;

	for (auto &child : bone->Children)
	{
		PrintSkeleton(child, depthCount);
	}
}

int Skeleton::GetKeyframe(Animation & animation, double time)
{
	if (time < 0)
		time = 0;
	if (time > animation.Duration)
		return animation.Keyframes.size() - 1;

	for (int keyframe = 0; keyframe < animation.Keyframes.size(); ++keyframe)
	{
		if (animation.Keyframes[keyframe].Time > time)
			return keyframe - 1;
	}

	return 0;
}
