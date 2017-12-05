#include "ModelLoader.h"

#include <iostream>
#include <fstream>

#include "ResourceManager.h"

string						ModelLoader::mDirectory;
std::vector<Texture2D>		ModelLoader::mTexturesLoaded;

void ModelLoader::Initialize()
{
}

void ModelLoader::Shutdown()
{
}

Model* ModelLoader::LoadModel(const char* file)
{
	PKAsset asset;

	ifstream fin;
	fin.open(file, ios::binary);

	readAsset(fin, asset);

	fin.close();

	int meshCount = asset.Scene.MeshCount;
	Mesh* meshes = new Mesh[meshCount];

	string path = file;
	path = path.substr(0, path.find_last_of("/"));
	mDirectory = path;

	for (int i = 0; i < meshCount; i++)
	{
		meshes[i] = loadMesh(&asset.Scene.Meshes[i], &asset);
	}

	return new Model(meshes, meshCount, mDirectory);
}

SkinnedModel* ModelLoader::LoadSkinnedModel(const char* file)
{
	PKAsset asset;

	ifstream fin;
	fin.open(file, ios::binary);

	readAsset(fin, asset);

	fin.close();

	//int meshCount = asset.Scene.MeshCount;
	//Mesh* meshes = new Mesh[meshCount];
	std::vector<Mesh> meshes;

	string path = file;
	path = path.substr(0, path.find_last_of("/"));
	mDirectory = path;

	for (int i = 0; i < asset.Scene.MeshCount; i++)
	{
		meshes.push_back(loadMesh(&asset.Scene.Meshes[i], &asset));
	}

	Skeleton* skeleton = loadSkeleton(&asset.Scene.Skeleton);

	return new SkinnedModel(meshes, skeleton, mDirectory);
}

ModelLoader::ModelLoader()
{
}

void ModelLoader::readAsset(ifstream& fin, PKAsset& asset)
{
	size_t size;

	// Asset Name
	fin.read(reinterpret_cast<char*>(&size), sizeof(size));
	asset.Name.resize(size);
	fin.read(reinterpret_cast<char*>(&asset.Name[0]), size);

	// Asset Version
	fin.read(reinterpret_cast<char*>(&size), sizeof(size));
	asset.Version.resize(size);
	fin.read(reinterpret_cast<char*>(&asset.Version[0]), size);

	// Asset Type
	fin.read(reinterpret_cast<char*>(&asset.Type), sizeof(asset.Type));

	// Scene
	fin.read(reinterpret_cast<char*>(&size), sizeof(size));
	asset.Scene.Name.resize(size);
	fin.read(reinterpret_cast<char*>(&asset.Scene.Name[0]), size);

	fin.read(reinterpret_cast<char*>(&asset.Scene.MeshCount), sizeof(asset.Scene.MeshCount));
	asset.Scene.Meshes.resize(asset.Scene.MeshCount);

	// Meshes
	for (int i = 0; i < asset.Scene.MeshCount; i++)
	{
		fin.read(reinterpret_cast<char*>(&size), sizeof(size));
		asset.Scene.Meshes[i].Name.resize(size);
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Name[0]), size);

		// Vertices
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].VertexCount), sizeof(asset.Scene.Meshes[i].VertexCount));
		asset.Scene.Meshes[i].Vertices.resize(asset.Scene.Meshes[i].VertexCount);

		for (int j = 0; j < asset.Scene.Meshes[i].VertexCount; j++)
		{
			fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Vertices[j]), sizeof(asset.Scene.Meshes[i].Vertices[j]));
		}

		// Indices
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].IndexCount), sizeof(asset.Scene.Meshes[i].IndexCount));
		asset.Scene.Meshes[i].Indices.resize(asset.Scene.Meshes[i].IndexCount);

		for (int j = 0; j < asset.Scene.Meshes[i].IndexCount; j++)
		{
			fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Indices[j]), sizeof(asset.Scene.Meshes[i].Indices[j]));
		}

		// Material
		fin.read(reinterpret_cast<char*>(&size), sizeof(size));
		asset.Scene.Meshes[i].Material.Name.resize(size);
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.Name[0]), size);

		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.Type), sizeof(asset.Scene.Meshes[i].Material.Type));
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.DiffuseColor), sizeof(asset.Scene.Meshes[i].Material.DiffuseColor));
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.SpecularColor), sizeof(asset.Scene.Meshes[i].Material.SpecularColor));
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.AmbientColor), sizeof(asset.Scene.Meshes[i].Material.AmbientColor));
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.Shininess), sizeof(asset.Scene.Meshes[i].Material.Shininess));
		fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.TextureCount), sizeof(asset.Scene.Meshes[i].Material.TextureCount));
		asset.Scene.Meshes[i].Material.Textures.resize(asset.Scene.Meshes[i].Material.TextureCount);

		for (int j = 0; j < asset.Scene.Meshes[i].Material.TextureCount; j++)
		{
			// Name
			fin.read(reinterpret_cast<char*>(&size), sizeof(size));
			asset.Scene.Meshes[i].Material.Textures[j].Name.resize(size);
			fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.Textures[j].Name[0]), size);

			// Type
			fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.Textures[j].Type), sizeof(asset.Scene.Meshes[i].Material.Textures[j].Type));

			// Path
			fin.read(reinterpret_cast<char*>(&size), sizeof(size));
			asset.Scene.Meshes[i].Material.Textures[j].Path.resize(size);
			fin.read(reinterpret_cast<char*>(&asset.Scene.Meshes[i].Material.Textures[j].Path[0]), size);
		}
	}

	// Skeleton
	// Bones
	fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.BoneCount), sizeof(asset.Scene.Skeleton.BoneCount));
	asset.Scene.Skeleton.Bones.resize(asset.Scene.Skeleton.BoneCount);

	for (int i = 0; i < asset.Scene.Skeleton.BoneCount; i++)
	{
		// Bone ID
		fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Bones[i].ID), sizeof(asset.Scene.Skeleton.Bones[i].ID));

		// Bone ParentID
		fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Bones[i].ParentID), sizeof(asset.Scene.Skeleton.Bones[i].ParentID));

		// Bone Name
		fin.read(reinterpret_cast<char*>(&size), sizeof(size));
		asset.Scene.Skeleton.Bones[i].Name.resize(size);
		fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Bones[i].Name[0]), size);

		// Bone OffsetMatrix
		fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Bones[i].OffsetMatrix), sizeof(asset.Scene.Skeleton.Bones[i].OffsetMatrix));
	}

	// Animations
	fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.AnimationCount), sizeof(asset.Scene.Skeleton.AnimationCount));
	asset.Scene.Skeleton.Animations.resize(asset.Scene.Skeleton.AnimationCount);

	for (int i = 0; i < asset.Scene.Skeleton.AnimationCount; i++)
	{
		// Animation Name
		fin.read(reinterpret_cast<char*>(&size), sizeof(size));
		asset.Scene.Skeleton.Animations[i].Name.resize(size);
		fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Animations[i].Name[0]), size);

		// Animation Duration
		fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Animations[i].Duration), sizeof(asset.Scene.Skeleton.Animations[i].Duration));

		// Animation Keyframes Count
		fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Animations[i].KeyframeCount), sizeof(asset.Scene.Skeleton.Animations[i].KeyframeCount));
		asset.Scene.Skeleton.Animations[i].Keyframes.resize(asset.Scene.Skeleton.Animations[i].KeyframeCount);

		// Animation Keyframes
		for (int j = 0; j < asset.Scene.Skeleton.Animations[i].KeyframeCount; j++)
		{
			// Keyframe Index
			fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Animations[i].Keyframes[j].Index), sizeof(asset.Scene.Skeleton.Animations[i].Keyframes[j].Index));

			// Keyframe Time
			fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Animations[i].Keyframes[j].Time), sizeof(asset.Scene.Skeleton.Animations[i].Keyframes[j].Time));

			// Keyframe BoneProperties Count
			fin.read(reinterpret_cast<char*>(&asset.Scene.Skeleton.Animations[i].Keyframes[j].BonePropertiesCount), sizeof(asset.Scene.Skeleton.Animations[i].Keyframes[j].BonePropertiesCount));
			
			// Keyframe BoneProperties
			for (int k = 0; k < asset.Scene.Skeleton.Animations[i].Keyframes[j].BonePropertiesCount; k++)
			{
				// BoneProperty Index
				int index;
				fin.read(reinterpret_cast<char*>(&index), sizeof(index));

				// BoneProperty
				PKBoneProperty property;
				fin.read(reinterpret_cast<char*>(&property), sizeof(property));

				asset.Scene.Skeleton.Animations[i].Keyframes[j].BoneProperties[index] = property;
			}
		}
	}
}

Mesh ModelLoader::loadMesh(PKMesh* mesh, const PKAsset* asset)
{
	// Load Material
	PKMaterial material = mesh->Material;
	std::vector<Texture2D> textures;

	for (int i = 0; i < material.TextureCount; i++)
	{
		PKTexture texture = material.Textures[i];

		bool skip = false;
		for (int j = 0; j < mTexturesLoaded.size(); j++)
		{
				string path = mDirectory + "/" + texture.Path;
				if (mTexturesLoaded[j].mPath == path)
				{
					textures.push_back(mTexturesLoaded[j]);
					skip = true;
					break;
			}
		}

		if (!skip)
		{
				string path = mDirectory + "/" + texture.Path;
				Texture2D tex = ResourceManager::LoadTexture2D(path.c_str(), GL_FALSE, texture.Name);
				tex.mType = texture.Type;

				textures.push_back(tex);
				mTexturesLoaded.push_back(tex);
		}
	}

	// Fill the vertex and bone data
	std::vector<Vertex> vertices;
	std::vector<BoneData> boneData;
	for (int i = 0; i < mesh->VertexCount; i++)
	{
		Vertex vertex;
		vertex.Position = mesh->Vertices[i].Position;
		vertex.Normal = mesh->Vertices[i].Normal;
		vertex.Tangent = mesh->Vertices[i].Tangent;
		vertex.BiTangent = mesh->Vertices[i].BiTangent;
		vertex.TexCoords = mesh->Vertices[i].TexCoords;
		vertices.push_back(vertex);

		BoneData bData;
		bData.BoneIndices1 = mesh->Vertices[i].BoneIndices1;
		bData.BoneIndices2 = mesh->Vertices[i].BoneIndices2;
		bData.BoneWeights1 = mesh->Vertices[i].BoneWeights1;
		bData.BoneWeights2 = mesh->Vertices[i].BoneWeights2;
		boneData.push_back(bData);
	}

	if (asset->Type == ASSET_TYPE_SKINNED_MODEL)
		return Mesh(vertices, mesh->Indices, textures, boneData);
	else
		return Mesh(vertices, mesh->Indices, textures);
}

Skeleton* ModelLoader::loadSkeleton(PKSkeleton* pkSkeleton)
{
	// Create the skeleton
	Skeleton* skeleton = new Skeleton();
	
	// Create the bones
	for (int i = 0; i < pkSkeleton->BoneCount; i++)
	{
		PKBone pkBone = pkSkeleton->Bones[i];
		skeleton->CreateBone(pkBone.ID, pkBone.ParentID, pkBone.Name, pkBone.OffsetMatrix);
	}

	// Initialize the animations
	for (int i = 0; i < pkSkeleton->AnimationCount; i++)
	{
		PKAnimation pkAnimation = pkSkeleton->Animations[i];
		Skeleton::Animation animation;
		string animationName;

		animationName = pkAnimation.Name;
		animation.Name = pkAnimation.Name;
		animation.Duration = pkAnimation.Duration;
		
		// Keyframes
		for (int j = 0; j < pkAnimation.KeyframeCount; j++)
		{
			PKKeyframe pkKeyframe = pkAnimation.Keyframes[j];
			Skeleton::Animation::Keyframe keyframe;

			keyframe.Index = pkKeyframe.Index;
			keyframe.Time = pkKeyframe.Time;

			// Bone Properties
			for (auto& pair : pkKeyframe.BoneProperties)
			{
				int index = pair.first;
				PKBoneProperty pkProperty = pair.second;

				Skeleton::Animation::Keyframe::BoneProperty property;
				property.ID = pkProperty.ID;
				property.Position = pkProperty.Position;
				property.Rotation = pkProperty.Rotation;
				property.Scale = pkProperty.Scale;

				keyframe.BoneProperties[index] = property;
			}

			animation.Keyframes.push_back(keyframe);
		}

		skeleton->mAnimations[animationName] = animation;
	}

	return skeleton;
}
