#pragma once

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <ASSIMP/Importer.hpp>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>
using namespace Assimp;

enum TextureType
{
	TEXTURE_TYPE_DIFFUSE,
	TEXTURE_TYPE_SPECULAR,
	TEXTURE_TYPE_AMBIENT,
	TEXTURE_TYPE_NORMAL,
	TEXTURE_TYPE_ALBEDO,
	TEXTURE_TYPE_ROUGHNESS,
	TEXTURE_TYPE_METALLIC,
	TEXTURE_TYPE_AO,
	TEXTURE_TYPE_UNDEFINED
};

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

typedef struct
{
	float x, y, z;
} VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
} FaceType;

struct Vertex
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
	vector<Vertex> Vertices;
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

class AssetConverter
{
public:
	static int GLConvertOBJ(string path, std::string& error);
	static int DXConvertOBJ(string path, std::string& error);
	//static int ValidateAsset(const char* path, std::string& error);

private:
	AssetConverter();

	static void processNode(aiNode* node, const aiScene* scene);
	static PKMesh processMesh(aiMesh* mesh, const aiScene* scene);
	static void processSkeleton(const aiScene* scene);
	static void processAnimations(const aiScene* scene);
	static vector<PKTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType type);
	static PKTexture loadTexture(string path, TextureType type);
	static void writeAssetToFile(ofstream& fout, PKAsset& asset);

	static void createSkeleton(vector<tuple<string, glm::mat4>>& boneInfo, std::map<std::string, int>&boneNameMapping, aiNode* node, int parentID);
	static int getBoneID(string boneName);
};

