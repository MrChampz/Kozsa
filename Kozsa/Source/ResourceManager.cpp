#include "ResourceManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include "ModelLoader.h"

// Instantiate static variables
std::map<std::string, Shader>			ResourceManager::mShaders;
std::map<std::string, Texture2D>		ResourceManager::mTextures;
std::map<std::string, TextureCubemap>	ResourceManager::mCubemaps;
std::map<std::string, Model*>			ResourceManager::mModels;
std::map<std::string, SkinnedModel*>	ResourceManager::mSkinnedModels;

void ResourceManager::Initialize()
{
	ModelLoader::Initialize();
}

void ResourceManager::Shutdown()
{
	ModelLoader::Shutdown();

	// Delete all resources loaded
	Clear();
}

Shader ResourceManager::LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name)
{
	mShaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return mShaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return mShaders[name];
}

Texture2D ResourceManager::LoadTexture2D(const GLchar* file, GLboolean alpha, std::string name)
{
	mTextures[name] = loadTextureFromFile(file, alpha);
	return mTextures[name];
}

Texture2D ResourceManager::GetTexture2D(std::string name)
{
	return mTextures[name];
}

Texture2D ResourceManager::LoadTextureHDR(const GLchar * file, GLboolean alpha, std::string name)
{
	mTextures[name] = loadTextureHDRFromFile(file, alpha);
	return mTextures[name];
}

Texture2D ResourceManager::GetTextureHDR(std::string name)
{
	return mTextures[name];
}

TextureCubemap ResourceManager::LoadTextureCubemap(const GLchar* path, GLboolean alpha, std::string name)
{
	mCubemaps[name] = loadTextureCubemapFromFile(path, alpha);
	return mCubemaps[name];
}

TextureCubemap ResourceManager::GetTextureCubemap(std::string name)
{
	return mCubemaps[name];
}

Model* ResourceManager::LoadModel(const GLchar* file, std::string name)
{
	mModels[name] = loadModelFromFile(file);
	return mModels[name];
}

Model* ResourceManager::GetModel(std::string name)
{
	return mModels[name];
}

SkinnedModel* ResourceManager::LoadSkinnedModel(const GLchar* file, std::string name)
{
	mSkinnedModels[name] = loadSkinnedModelFromFile(file);
	return mSkinnedModels[name];
}

SkinnedModel* ResourceManager::GetSkinnedModel(std::string name)
{
	return mSkinnedModels[name];
}

void ResourceManager::Clear()
{
	// (Properly) delete all shaders
	for (auto iter : mShaders)
		glDeleteProgram(iter.second.mID);

	// (Properly) delete all textures
	for (auto iter : mTextures)
		glDeleteTextures(1, &iter.second.mID);

	// (Properly) delete all cubemap textures
	for (auto iter : mCubemaps)
		glDeleteTextures(1, &iter.second.mID);

	// TODO: Implementar o gerenciamento de memória dos models aqui também.
}

ResourceManager::ResourceManager()
{
}

Shader ResourceManager::loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile)
{
	// Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		// Open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;

		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();

		// Close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();

		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		// If geometry shader path is present, also load a geometry shader
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();

	// Now create shader object from source code
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);

	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar* file, GLboolean alpha)
{
	// Create texture object
	Texture2D texture;
	if(alpha)
	{
		texture.mInternalFormat = GL_RGBA;
		texture.mImageFormat = GL_RGBA;
	}

	// Store the image path
	texture.mPath = file;

	// Load image
	int width, height, bpp;
	unsigned char* image = stbi_load(file, &width, &height, &bpp, texture.mImageFormat == GL_RGBA ? 4 : 3);

	// Now generate texture
	texture.Generate(width, height, image);

	// And finally free image data
	stbi_image_free(image);

	return texture;
}

Texture2D ResourceManager::loadTextureHDRFromFile(const GLchar * file, GLboolean alpha)
{
	// Create texture object
	Texture2D texture;
	if (alpha)
	{
		texture.mInternalFormat = GL_RGBA;
		texture.mImageFormat = GL_RGBA;
	}

	// Store the image path
	texture.mPath = file;

	// Set the wrap to GL_CLAMP_TO_EDGE
	texture.mWrapS = GL_CLAMP_TO_EDGE;
	texture.mWrapT = GL_CLAMP_TO_EDGE;

	// Load image
	int width, height, bpp;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image = stbi_load(file, &width, &height, &bpp, texture.mImageFormat == GL_RGBA ? 4 : 3);

	// Now generate texture
	texture.Generate(width, height, image);

	// And finally free image data
	stbi_image_free(image);

	return texture;
}

TextureCubemap ResourceManager::loadTextureCubemapFromFile(const GLchar* path, GLboolean alpha)
{
	// Define the name of the images
	vector<string> files;
	files.push_back("right.tga");
	files.push_back("left.tga");
	files.push_back("top.tga");
	files.push_back("bottom.tga");
	files.push_back("back.tga");
	files.push_back("front.tga");

	// Create cubemap texture object
	TextureCubemap cubemap;
	if (alpha)
	{
		cubemap.mInternalFormat = GL_RGBA;
		cubemap.mImageFormat = GL_RGBA;
	}

	// Store the images path
	cubemap.mPath = path;

	// Load images
	int width, height, bpp;
	vector<unsigned char*> images;

	for (int i = 0; i < files.size(); i++)
		images.push_back(stbi_load((path + files[i]).c_str(), &width, &height, &bpp, cubemap.mImageFormat == GL_RGBA ? 4 : 3));

	// Now generate cubemap texture
	cubemap.Generate(width, height, images);

	// And finally free images data
	for (int i = 0; i < images.size(); i++)
		stbi_image_free(images[i]);

	return cubemap;
}

Model* ResourceManager::loadModelFromFile(const GLchar* file)
{

	return ModelLoader::LoadModel(file);
}

SkinnedModel* ResourceManager::loadSkinnedModelFromFile(const GLchar* file)
{

	return ModelLoader::LoadSkinnedModel(file);
}
