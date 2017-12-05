#include "stdafx.h"

#include "AssetConverter.h"

#include <iostream>
#include <fstream>
#include <stdio.h>

static string mDirectory;
static vector<PKMesh> mMeshes;
static PKSkeleton mSkeleton;

static vector<tuple<string, glm::mat4>> boneInfo;
static map<string, int> boneNameMapping;
static AssetType assetType;

int AssetConverter::GLConvertOBJ(string path, std::string& error)
{
	Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		error = "Erro do Assimp!";
		cout << "ERROR::ASSIMP: " << importer.GetErrorString() << endl;
		return -1;
	}

	mDirectory = path.substr(0, path.find_last_of('/'));
	assetType = ASSET_TYPE_MODEL;

	processNode(scene->mRootNode, scene);
	processSkeleton(scene);
	processAnimations(scene);

	error = "";

	// Scene
	PKScene pkScene;
	pkScene.Name = "Cena de Teste";
	pkScene.MeshCount = mMeshes.size();
	
	// Adding meshes
	for (int i = 0; i < pkScene.MeshCount; i++)
		pkScene.Meshes.push_back(mMeshes[i]);

	// Adding skeleton
	pkScene.Skeleton = mSkeleton;

	// Asset
	PKAsset asset;
	asset.Name = "Asset de Teste";
	asset.Version = "1.0.1";
	asset.Type = assetType;
	asset.Scene = pkScene;

	ofstream fout;
	string outPath = path.substr(0, path.find_last_of('.')) + ".pkasset";

	fout.open(outPath.c_str(), ios::binary);
	if (!fout)
	{
		error = "Erro ao gravar arquivo PKAsset!";
		return -1;
	}

	writeAssetToFile(fout, asset);

	fout.close();

	error = "Arquivo salvo em: " + outPath;

	return 0;
}

int AssetConverter::DXConvertOBJ(string path, std::string& error)
{
	Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		error = "Erro do Assimp!";
		cout << "ERROR::ASSIMP: " << importer.GetErrorString() << endl;
		return -1;
	}

	mDirectory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	error = "";

	// Scene
	PKScene pkScene;
	pkScene.Name = "Cena de Teste";
	pkScene.MeshCount = mMeshes.size();

	// Adding meshes
	for (int i = 0; i < pkScene.MeshCount; i++)
		pkScene.Meshes.push_back(mMeshes[i]);

	// Asset
	PKAsset asset;
	asset.Name = "Asset de Teste";
	asset.Scene = pkScene;

	ofstream fout;
	string outPath = path.substr(0, path.find_last_of('.')) + ".pkasset";

	fout.open(outPath.c_str(), ios::binary);
	if (!fout)
	{
		error = "Erro ao gravar arquivo PKAsset!";
		return -1;
	}

	writeAssetToFile(fout, asset);

	fout.close();

	error = "Arquivo salvo em: " + outPath;

	return 0;
}

void AssetConverter::processNode(aiNode * node, const aiScene * scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		mMeshes.push_back(processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

PKMesh AssetConverter::processMesh(aiMesh * mesh, const aiScene * scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	PKMaterial* material = new PKMaterial;

	// Walk through each of the mesh's vertices
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
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
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		// Retrieve all indices of the face and store them in the indices vector
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Calculate normal mapping tangents
	for (int i = 0; i < indices.size(); i += 3) {
		Vertex& v0 = vertices[indices[i]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		glm::vec3 edge1 = v1.Position - v0.Position;
		glm::vec3 edge2 = v2.Position - v0.Position;

		float deltaU1 = v1.TexCoords.x - v0.TexCoords.x;
		float deltaV1 = v1.TexCoords.y - v0.TexCoords.y;
		float deltaU2 = v2.TexCoords.x - v0.TexCoords.x;
		float deltaV2 = v2.TexCoords.y - v0.TexCoords.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

		glm::vec3 tangent;
		tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
		tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
		tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

		v0.Tangent += tangent;
		v1.Tangent += tangent;
		v2.Tangent += tangent;
	}
	for (auto& vertex : vertices) {
		vertex.Tangent = glm::normalize(vertex.Tangent);
		vertex.BiTangent = glm::normalize(glm::cross(vertex.Tangent, glm::normalize(vertex.Normal)));
	}

	// Bone Data
	if (mesh->HasBones())
	{
		// If has bones, set the asset type to skinned model.
		assetType = ASSET_TYPE_SKINNED_MODEL;

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

		// Load bone indices and weights
		for (auto& pair : vertexWeights)
		{
			auto weights = pair.second;
			Vertex& desc = vertices[pair.first];

			const int maxWeights = 8;
			if (weights.size() > maxWeights)
			{
				std::cout << "Vertex weights (" << weights.size() << ") greater than max weights per vertex (" << maxWeights << ")" << std::endl;
			}
			for (int weightIndex = 0; weightIndex < weights.size() && weightIndex < maxWeights && weightIndex < 4; ++weightIndex)
			{
				std::tie(desc.BoneIndices1[weightIndex], desc.BoneWeights1[weightIndex]) = weights[weightIndex];
			}
			for (int weightIndex = 4; weightIndex < weights.size() && weightIndex < maxWeights && weightIndex < 8; ++weightIndex)
			{
				std::tie(desc.BoneIndices2[weightIndex - 4], desc.BoneWeights2[weightIndex - 4]) = weights[weightIndex];
			}
		}
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		vector<PKTexture> textures;

		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		aiString name;
		aiColor3D diffuseColor;
		aiColor3D specularColor;
		aiColor3D ambientColor;
		float shininess;

		// Get material properties
		mat->Get(AI_MATKEY_NAME, name);
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		mat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		mat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
		mat->Get(AI_MATKEY_SHININESS, shininess);

		// Verifies if the material has diffuse, specular, ambient, and others standard shader texture channels
		if (mat->GetTextureCount(aiTextureType_DIFFUSE)		> 0 && 
			mat->GetTextureCount(aiTextureType_NORMALS)		> 0 &&
			mat->GetTextureCount(aiTextureType_SPECULAR)	> 0 && 
			mat->GetTextureCount(aiTextureType_AMBIENT)		> 0
			)
		{
			// Diffuse maps
			vector<PKTexture> diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, TEXTURE_TYPE_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// Normal maps
			vector<PKTexture> normalMaps = loadMaterialTextures(mat, aiTextureType_NORMALS, TEXTURE_TYPE_NORMAL);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			// Specular maps
			vector<PKTexture> specularMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR, TEXTURE_TYPE_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			// Ambient maps
			vector<PKTexture> ambientMaps = loadMaterialTextures(mat, aiTextureType_AMBIENT, TEXTURE_TYPE_AMBIENT);
			textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());

			material->Type = MATERIAL_TYPE_PHONG;
		}
		else
		{
			int opt;
			string path;

			cout << "Nenhuma textura encontrada para o material " << name.C_Str()
				 << "! Digite alguma opção para tipo de material: " << endl;
			cout << "[0] para Material Standard-Phong" << endl;
			cout << "[1] para Material PBR" << endl;
			cin >> opt;

			// Standard-Phong Material
			if (opt == 0)
			{
				// Diffuse Map
				cout << endl;
				cout << "Diffuse Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_DIFFUSE));
				path.clear();

				// Normal Map
				cout << endl;
				cout << "Normal Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_NORMAL));
				path.clear();

				// Specular Map
				cout << endl;
				cout << "Specular Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_SPECULAR));
				path.clear();

				// Ambient Map
				cout << endl;
				cout << "Ambient Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_AMBIENT));
				path.clear();

				material->Type = MATERIAL_TYPE_PHONG;
			}
			// PBR Material
			else if (opt == 1)
			{
				// Albedo Map
				cout << endl;
				cout << "Albedo Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_ALBEDO));
				path.clear();

				// Normal Map
				cout << endl;
				cout << "Normal Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_NORMAL));
				path.clear();

				// Roughness Map
				cout << endl;
				cout << "Roughness Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_ROUGHNESS));
				path.clear();

				// Metallic Map
				cout << endl;
				cout << "Metallic Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_METALLIC));
				path.clear();

				// Ambient Occlusion (AO) Map
				cout << endl;
				cout << "Ambient Occlusion (AO) Map" << endl;
				cout << "Digite o caminho até a textura: " << endl;
				cin >> path;
				textures.push_back(loadTexture(path, TEXTURE_TYPE_AO));
				path.clear();

				material->Type = MATERIAL_TYPE_PBR;
			}
			else
			{
				cout << endl;
				cout << "Opção incorreta!" << endl;
				cout << endl;
				system("pause");
			}
		}

		// Fill the material struct with data
		material->Name = name.C_Str();
		material->DiffuseColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
		material->SpecularColor = glm::vec3(specularColor.r, specularColor.g, specularColor.b);;
		material->AmbientColor = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);;
		material->Shininess = shininess;
		material->TextureCount = textures.size();
		for (int i = 0; i < textures.size(); i++)
		{
			material->Textures.push_back(textures[i]);
		}
	}

	// Fill the mesh struct with data
	PKMesh pkMesh;
	pkMesh.Name = mesh->mName.C_Str();
	pkMesh.Material = *material;
	pkMesh.VertexCount = vertices.size();
	pkMesh.IndexCount = indices.size();
	
	for (int i = 0; i < pkMesh.VertexCount; i++)
		pkMesh.Vertices.push_back(vertices[i]);

	for (int i = 0; i < pkMesh.IndexCount; i++)
		pkMesh.Indices.push_back(indices[i]);

	return pkMesh;
}

void AssetConverter::processSkeleton(const aiScene * scene)
{
	if (!boneInfo.empty())
	{
		createSkeleton(boneInfo, boneNameMapping, scene->mRootNode, -1);
	}
}

void AssetConverter::processAnimations(const aiScene * scene)
{
	for (int i = 0; i < scene->mNumAnimations; ++i)
	{
		auto animation = scene->mAnimations[i];
		string animationName = animation->mName.C_Str();

		PKAnimation pkAnim;
		pkAnim.Name = animationName;
		pkAnim.Duration = animation->mDuration / animation->mTicksPerSecond;

		map<int, double> frameTimes;
		map<int, map<int, PKBoneProperty>> frameBoneProperties;

		for (int channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex)
		{
			auto channel = animation->mChannels[channelIndex];
			std::string boneName = channel->mNodeName.C_Str();
			int boneID = getBoneID(boneName);
			if (boneID == -1)
			{
				continue;
			}

			if (channel->mNumPositionKeys != channel->mNumRotationKeys || channel->mNumPositionKeys != channel->mNumScalingKeys)
			{
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
			PKKeyframe animationFrame;
			animationFrame.Index = keyframe;
			animationFrame.Time = frameTimes[keyframe] / animation->mTicksPerSecond;
			for (auto& kv2 : kv.second)
			{
				int boneID = kv2.first;
				auto& property = kv2.second;
				animationFrame.BonePropertiesCount += 1;
				animationFrame.BoneProperties[boneID] = property;
			}
			animationFrame.BonePropertiesCount = animationFrame.BoneProperties.size();

			pkAnim.Keyframes.push_back(animationFrame);
		}
		pkAnim.KeyframeCount = pkAnim.Keyframes.size();

		mSkeleton.AnimationCount++;
		mSkeleton.Animations.push_back(pkAnim);
	}
}

vector<PKTexture> AssetConverter::loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType type)
{
	vector<PKTexture> textures;
	for (int i = 0; i < mat->GetTextureCount(aiType); i++)
	{
		aiString str;
		mat->GetTexture(aiType, i, &str);
		string path = str.C_Str();

		PKTexture tex;
		tex.Name = (path.substr(path.find_last_of('\\') + 1)).length(), (path.substr(path.find_last_of('\\') + 1)).c_str();
		tex.Path = (path.substr(path.find_last_of('\\') + 1)).length(), (path.substr(path.find_last_of('\\') + 1)).c_str();
		tex.Type = type;
		textures.push_back(tex);
	}

	return textures;
}

PKTexture AssetConverter::loadTexture(string path, TextureType type)
{
	string name = (path.substr(path.find_last_of('/') + 1, ((path.find_last_of('.') - 1) - path.find_last_of('/')))).c_str();

	PKTexture tex;
	tex.Name = name;
	tex.Path = path;
	tex.Type = type;

	return tex;
}

void AssetConverter::writeAssetToFile(ofstream& fout, PKAsset& asset)
{
	size_t size;

	// Asset Name
	size = asset.Name.size();
	fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
	fout.write(reinterpret_cast<const char*>(&asset.Name[0]), size);

	// Asset Version
	size = asset.Version.size();
	fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
	fout.write(reinterpret_cast<const char*>(&asset.Version[0]), size);

	// Asset Type
	fout.write(reinterpret_cast<const char*>(&asset.Type), sizeof(asset.Type));

	// Scene
	size = asset.Scene.Name.size();
	fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
	fout.write(reinterpret_cast<const char*>(&asset.Scene.Name[0]), size);
	fout.write(reinterpret_cast<const char*>(&asset.Scene.MeshCount), sizeof(asset.Scene.MeshCount));

	// Meshes
	for (int i = 0; i < asset.Scene.MeshCount; i++)
	{
		size = asset.Scene.Meshes[i].Name.size();
		fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Name[0]), size);

		// Vertices
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].VertexCount), sizeof(asset.Scene.Meshes[i].VertexCount));

		for (int j = 0; j < asset.Scene.Meshes[i].VertexCount; j++)
		{
			fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Vertices[j]), sizeof(asset.Scene.Meshes[i].Vertices[j]));
		}

		// Indices
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].IndexCount), sizeof(asset.Scene.Meshes[i].IndexCount));

		for (int j = 0; j < asset.Scene.Meshes[i].IndexCount; j++)
		{
			fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Indices[j]), sizeof(asset.Scene.Meshes[i].Indices[j]));
		}

		// Material
		size = asset.Scene.Meshes[i].Material.Name.size();
		fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.Name[0]), size);

		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.Type), sizeof(asset.Scene.Meshes[i].Material.Type));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.DiffuseColor), sizeof(asset.Scene.Meshes[i].Material.DiffuseColor));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.SpecularColor), sizeof(asset.Scene.Meshes[i].Material.SpecularColor));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.AmbientColor), sizeof(asset.Scene.Meshes[i].Material.AmbientColor));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.Shininess), sizeof(asset.Scene.Meshes[i].Material.Shininess));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.TextureCount), sizeof(asset.Scene.Meshes[i].Material.TextureCount));

		for (int j = 0; j < asset.Scene.Meshes[i].Material.TextureCount; j++)
		{	
			// Name
			size = asset.Scene.Meshes[i].Material.Textures[j].Name.size();
			fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
			fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.Textures[j].Name[0]), size);

			// Type
			fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.Textures[j].Type), sizeof(asset.Scene.Meshes[i].Material.Textures[j].Type));

			// Path
			size = asset.Scene.Meshes[i].Material.Textures[j].Path.size();
			fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
			fout.write(reinterpret_cast<const char*>(&asset.Scene.Meshes[i].Material.Textures[j].Path[0]), size);
		}
	}

	// Skeleton
	// Bones
	fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.BoneCount), sizeof(asset.Scene.Skeleton.BoneCount));

	for (int i = 0; i < asset.Scene.Skeleton.BoneCount; i++)
	{
		// Bone ID
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Bones[i].ID), sizeof(asset.Scene.Skeleton.Bones[i].ID));

		// Bone ParentID
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Bones[i].ParentID), sizeof(asset.Scene.Skeleton.Bones[i].ParentID));

		// Bone Name
		size = asset.Scene.Skeleton.Bones[i].Name.size();
		fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Bones[i].Name[0]), size);

		// Bone OffsetMatrix
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Bones[i].OffsetMatrix), sizeof(asset.Scene.Skeleton.Bones[i].OffsetMatrix));
	}

	// Animations
	fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.AnimationCount), sizeof(asset.Scene.Skeleton.AnimationCount));

	for (int i = 0; i < asset.Scene.Skeleton.AnimationCount; i++)
	{
		// Animation Name
		size = asset.Scene.Skeleton.Animations[i].Name.size();
		fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Animations[i].Name), size);

		// Animation Duration
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Animations[i].Duration), sizeof(asset.Scene.Skeleton.Animations[i].Duration));

		// Animation Keyframes Count
		fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Animations[i].KeyframeCount), sizeof(asset.Scene.Skeleton.Animations[i].KeyframeCount));

		// Animation Keyframes
		for (int j = 0; j < asset.Scene.Skeleton.Animations[i].KeyframeCount; j++)
		{
			// Keyframe Index
			fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Animations[i].Keyframes[j].Index), sizeof(asset.Scene.Skeleton.Animations[i].Keyframes[j].Index));
			
			// Keyframe Time
			fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Animations[i].Keyframes[j].Time), sizeof(asset.Scene.Skeleton.Animations[i].Keyframes[j].Time));

			// Keyframe BoneProperties Count
			fout.write(reinterpret_cast<const char*>(&asset.Scene.Skeleton.Animations[i].Keyframes[j].BonePropertiesCount), sizeof(asset.Scene.Skeleton.Animations[i].Keyframes[j].BonePropertiesCount));

			// Keyframe BoneProperties
			for (auto& pair : asset.Scene.Skeleton.Animations[i].Keyframes[j].BoneProperties)
			{
				// BoneProperty Index
				fout.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));

				// BoneProperty
				fout.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
			}
		}
	}
}

void AssetConverter::createSkeleton(vector<tuple<string, glm::mat4>>& boneInfo, std::map<std::string, int>& boneNameMapping, aiNode * node, int parentID)
{
	std::string nodeName = node->mName.C_Str();

	if (boneNameMapping.find(nodeName) == boneNameMapping.end())
	{
		// Do nothing...
	}
	else
	{
		glm::mat4 offsetMatrix;
		int ID = boneNameMapping[nodeName];
		tie(ignore, offsetMatrix) = boneInfo[ID];

		PKBone bone;
		bone.ID = ID;
		bone.ParentID = parentID;
		bone.Name = nodeName;
		bone.OffsetMatrix = offsetMatrix;
		mSkeleton.Bones.push_back(bone);
		mSkeleton.BoneCount++;
		parentID = ID;
	}

	for (int childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		aiNode* child = node->mChildren[childIndex];
		createSkeleton(boneInfo, boneNameMapping, child, parentID);
	}
}

int AssetConverter::getBoneID(string boneName)
{
	int id = -1;

	for (int i = 0; i < mSkeleton.BoneCount; i++)
	{
		if (mSkeleton.Bones[i].Name == boneName)
		{
			id = mSkeleton.Bones[i].ID;
		}
	}

	return id;
}
