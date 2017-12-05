#include "SkinnedModel.h"

#include <iostream>
#include <string>

std::vector<std::tuple<std::string, glm::mat4>> boneInfo;
std::map<std::string, int> boneNameMapping;

SkinnedModel::SkinnedModel(std::string filepath)
{
	loadModel(filepath);
}

SkinnedModel::SkinnedModel(std::vector<Mesh> meshes, Skeleton* skeleton, std::string directory)
	: mMeshes(meshes), mSkeleton(skeleton), mDirectory(directory)
{

}

SkinnedModel::~SkinnedModel()
{
}

void SkinnedModel::Draw(Shader& shader)
{
	//std::vector<glm::mat4> frameBones = mSkeleton->GetFrameBones(mCurrentAnimation, _AnimationTime, mNoRootMotion);
	std::vector<glm::mat4> frameBones = mSkeleton->Blend(mCurrentAnimation, "Walk", _AnimationTime, mNoRootMotion);
	//glUniformMatrix4fv(glGetUniformLocation(shader.mID, "Bones"), frameBones.size(), GL_FALSE, glm::value_ptr(frameBones[0]));

	// Render each mesh of this model
	for (GLuint i = 0; i < this->mMeshes.size(); i++)
		this->mMeshes[i].Draw(shader);
}

void SkinnedModel::loadModel(std::string filepath)
{
	// ---------- ASSIMP ---------
	Importer importer;
	const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP: " << importer.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene);
	processSkeleton(scene);
	processAnimations(scene);
}

void SkinnedModel::processNode(aiNode * node, const aiScene * scene)
{
	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes)
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		//mMeshes.push_back(this->processMesh(mesh, scene));
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

AnimatedMesh SkinnedModel::processMesh(aiMesh * mesh, const aiScene * scene)
{
	// Data to fill
	std::vector<AnimatedVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture2D> textures;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		AnimatedVertex vertex;
		glm::vec3 vector;

		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		// Texture Coordinates
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// Now walk through each of the mesh's faces (a face is a mes its triangle) and retrieve the corresponding vertex indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Bone Data
	if (mesh->HasBones())
	{
		// Bones
		std::map<int, std::vector<std::tuple<int, float>>> vertexWeights;
		for (int i = 0; i < mesh->mNumBones; ++i)
		{
			auto bone = mesh->mBones[i];
			std::string boneName = bone->mName.C_Str();

			auto mat = bone->mOffsetMatrix;
			glm::mat4 glmMat
			(
				mat.a1, mat.b1, mat.c1, mat.d1,
				mat.a2, mat.b2, mat.c2, mat.d2,
				mat.a3, mat.b3, mat.c3, mat.d3,
				mat.a4, mat.b4, mat.c4, mat.d4
			);

			int boneIndex;
			if (boneNameMapping.find(boneName) != boneNameMapping.end())
			{
				boneIndex = boneNameMapping[boneName];
			}
			else
			{
				boneIndex = boneInfo.size();
				boneInfo.push_back(std::make_tuple(boneName, glmMat));
				boneNameMapping[boneName] = boneIndex;
			}
		
			for (int j = 0; j < bone->mNumWeights; ++j)
			{
				auto weight = bone->mWeights[j];
				unsigned int offsetVertexId = weight.mVertexId;
				vertexWeights[offsetVertexId].push_back(std::make_tuple(boneIndex, weight.mWeight));
			}
		}

		for (auto& pair : vertexWeights)
		{
			auto weights = pair.second;
			AnimatedVertex& desc = vertices[pair.first];
			
			const int maxWeights = 8;
			if (weights.size() > maxWeights)
			{
				std::cout << "Vertex weights (" << weights.size() << ") greater than max weights per vertex (" << maxWeights << ")" << std::endl;
			}
			for (int weightIndex = 0; weightIndex < weights.size() && weightIndex < maxWeights && weightIndex < 4; ++weightIndex)
			{
				std::tie(desc.IDs1[weightIndex], desc.Weights1[weightIndex]) = weights[weightIndex];
			}
			for (int weightIndex = 4; weightIndex < weights.size() && weightIndex < maxWeights && weightIndex < 8; ++weightIndex)
			{
				std::tie(desc.IDs2[weightIndex - 4], desc.Weights2[weightIndex - 4]) = weights[weightIndex];
			}
		}
	}

	// Return a mesh object created from the extracted mesh data
	return AnimatedMesh(vertices, indices, textures);
}

void SkinnedModel::processSkeleton(const aiScene * scene)
{
	if (!boneInfo.empty())
	{
		mSkeleton = new Skeleton();
		createSkeleton(boneInfo, boneNameMapping, scene->mRootNode, -1);
		int numBones = mSkeleton->mBones.size();
		std::cout << "Bone Count: " << numBones << std::endl;
		if (numBones > 0)
		{
			mSkeleton->PrintSkeleton();
		}
	}
}

void SkinnedModel::processAnimations(const aiScene * scene)
{
	std::cout << "Animation Count: " << scene->mNumAnimations << std::endl;
	
	for (int i = 0; i < scene->mNumAnimations; ++i)
	{
		auto animation = scene->mAnimations[i];
		std::string animationName = animation->mName.C_Str();
		std::cout << "Animation: " << animationName << std::endl;
		std::cout << "Duration: " << animation->mDuration << std::endl;
		std::cout << "Ticks per second: " << animation->mTicksPerSecond << std::endl;

		Skeleton::Animation skelAnim;
		skelAnim.Name = animationName;
		skelAnim.Duration = animation->mDuration / animation->mTicksPerSecond;

		std::map<int, double> frameTimes;
		std::map<int, std::map<int, Skeleton::Animation::Keyframe::BoneProperty>> frameBoneProperties;

		for (int channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex)
		{
			auto channel = animation->mChannels[channelIndex];
			std::string boneName = channel->mNodeName.C_Str();
			int boneID = mSkeleton->GetBoneID(boneName);
			if (boneID == -1)
			{
				std::cout << "Animation referenced a bone that doesn't exist: " << boneName << std::endl;
				continue;
			}

			if (channel->mNumPositionKeys != channel->mNumRotationKeys || channel->mNumPositionKeys != channel->mNumScalingKeys)
			{
				std::cout << "Hey animation " << animationName << ". You're dumb!" << std::endl;
				continue;
			}

			for (int keyframe = 0; keyframe < channel->mNumPositionKeys; ++keyframe)
			{
				auto posKey = channel->mPositionKeys[keyframe];
				auto rotKey = channel->mRotationKeys[keyframe];
				auto scaleKey = channel->mScalingKeys[keyframe];

				frameTimes[keyframe] = posKey.mTime;

				auto& property = frameBoneProperties[keyframe][boneID];
				property.ID = keyframe;
				property.Position = glm::vec3(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z);
				property.Rotation = glm::quat(rotKey.mValue.w, rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z);
				property.Scale = glm::vec3(scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z);
			}
		}

		for (auto& kv : frameBoneProperties)
		{
			int keyframe = kv.first;
			Skeleton::Animation::Keyframe animationFrame;
			animationFrame.Index = keyframe;
			animationFrame.Time = frameTimes[keyframe] / animation->mTicksPerSecond;
			for (auto& kv2 : kv.second)
			{
				int boneID = kv2.first;
				auto& property = kv2.second;
				animationFrame.BoneProperties[boneID] = property;
			}
			skelAnim.Keyframes.push_back(animationFrame);
		}

		mSkeleton->mAnimations[animationName] = skelAnim;
	}
}

void SkinnedModel::createSkeleton(std::vector<std::tuple<std::string, glm::mat4>>& boneInfo, std::map<std::string, int>& boneNameMapping, aiNode * node, int parentID)
{
	std::string nodeName = node->mName.C_Str();

	if (boneNameMapping.find(nodeName) == boneNameMapping.end())
	{
		std::cout << "Node \"" << nodeName << "\" was not a bone" << std::endl;
	}
	else
	{
		glm::mat4 offsetMatrix;
		int ID = boneNameMapping[nodeName];
		std::tie(std::ignore, offsetMatrix) = boneInfo[ID];
		mSkeleton->CreateBone(ID, parentID, nodeName, offsetMatrix);
		parentID = ID;
	}

	for (int childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		aiNode* child = node->mChildren[childIndex];
		createSkeleton(boneInfo, boneNameMapping, child, parentID);
	}
}
