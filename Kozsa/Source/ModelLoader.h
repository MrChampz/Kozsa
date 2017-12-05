#pragma once

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include "Model.h"
#include "SkinnedModel.h"
#include "Texture2D.h"

enum MaterialType
{
	MATERIAL_TYPE_BLINN,
	MATERIAL_TYPE_PHONG,
	MATERIAL_TYPE_PBR
};

enum AssetType
{
	ASSET_TYPE_MODEL,
	ASSET_TYPE_SKINNED_MODEL,
	ASSET_TYPE_SKELETON,
	ASSET_TYPE_ANIMATION
};

struct PKVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 BiTangent;
	glm::vec2 TexCoords;
	glm::vec4 BoneIndices1;
	glm::vec4 BoneIndices2;
	glm::vec4 BoneWeights1;
	glm::vec4 BoneWeights2;
};

struct PKTexture
{
	string Name;
	TextureType Type;
	string Path;
};

struct PKMaterial
{
	string Name;
	MaterialType Type;
	glm::vec3 DiffuseColor;
	glm::vec3 SpecularColor;
	glm::vec3 AmbientColor;
	float Shininess;
	int TextureCount;
	vector<PKTexture> Textures;
};

struct PKMesh
{
	string Name;
	int VertexCount;
	vector<PKVertex> Vertices;
	int IndexCount;
	vector<unsigned int> Indices;
	PKMaterial Material;
};

struct PKBoneProperty
{
	int ID;
	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;
};

struct PKKeyframe
{
	int Index;
	double Time;
	int BonePropertiesCount;
	map<int, PKBoneProperty> BoneProperties;
};

struct PKAnimation
{
	string Name;
	double Duration;
	int KeyframeCount;
	vector<PKKeyframe> Keyframes;
};

struct PKBone
{
	int ID;
	int ParentID;
	string Name;
	glm::mat4 OffsetMatrix;
};

struct PKSkeleton
{
	int BoneCount;
	vector<PKBone> Bones;
	int AnimationCount;
	vector<PKAnimation> Animations;
};

struct PKScene
{
	string Name;
	int MeshCount;
	vector<PKMesh> Meshes;
	PKSkeleton Skeleton;
};

struct PKAsset
{
	string Name;
	string Version;
	AssetType Type;
	PKScene Scene;
};

class ModelLoader
{
private:
	static string mDirectory;
	static std::vector<Texture2D> mTexturesLoaded;

public:

	static void Initialize();
	static void Shutdown();
	static Model* LoadModel(const char* file);
	static SkinnedModel* LoadSkinnedModel(const char* file);

private:
	// Private constructor
	ModelLoader();

private:
	static void readAsset(ifstream& fin, PKAsset& asset);
	static Mesh loadMesh(PKMesh* mesh, const PKAsset* asset);
	static Skeleton* loadSkeleton(PKSkeleton* pkSkeleton);
};