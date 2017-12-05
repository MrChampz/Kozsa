#pragma once

#include <map>
#include <string>

#include <GL/glew.h>

#include "Texture2D.h"
#include "TextureCubemap.h"
#include "Model.h"
#include "SkinnedModel.h"
#include "Shader.h"

/**
 * A static singleton ResourceManager class that hosts several
 * functions to load Textures, Models, and Shaders. Each loaded texture
 * and model handles. All functions and resources are static and no
 * public constructor is defined.
 */
class ResourceManager
{
private:
	// Resource storage
	static std::map<std::string, Shader>			mShaders;
	static std::map<std::string, Texture2D>			mTextures;
	static std::map<std::string, TextureCubemap>	mCubemaps;
	static std::map<std::string, Model*>			mModels;
	static std::map<std::string, SkinnedModel*>		mSkinnedModels;

public:
	static void Initialize();
	static void Shutdown();

	//Load (and generate) a shader program from file loading vertex, fragment (and geometry) shader's source code
	// If gShaderFile is not nullptr, it also loads a geometry shader
	static Shader LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name);

	// Retrieves a stored shader
	static Shader GetShader(std::string name);

	// Load (and generate) a texture from file
	static Texture2D LoadTexture2D(const GLchar* file, GLboolean alpha, std::string name);

	// Retrieve a stored texture
	static Texture2D GetTexture2D(std::string name);

	// Load (and generate) a HDR texture from file
	static Texture2D LoadTextureHDR(const GLchar* file, GLboolean alpha, std::string name);

	// Retrieve a stored HDR texture
	static Texture2D GetTextureHDR(std::string name);

	// Load (and generate) a cubemap texture from files
	static TextureCubemap LoadTextureCubemap(const GLchar* path, GLboolean alpha, std::string name);

	// Retrieve a stored cubemap texture
	static TextureCubemap GetTextureCubemap(std::string name);

	// Load (and generate) a model from file
	static Model* LoadModel(const GLchar* file, std::string name);

	// Retrieve a stored model
	static Model* GetModel(std::string name);

	// Load (and generate) a skinned model from file
	static SkinnedModel* LoadSkinnedModel(const GLchar* file, std::string name);

	// Retrieve a stored skinned model
	static SkinnedModel* GetSkinnedModel(std::string name);

	// Properly de-allocates all loaded resources
	static void Clear();

private:
	// Private constructor
	ResourceManager();

private:
	// Loads and generates a shader from file
	static Shader loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile = nullptr);

	// Loads a single texture from file
	static Texture2D loadTextureFromFile(const GLchar* file, GLboolean alpha);

	// Loads a single HDR texture from file
	static Texture2D loadTextureHDRFromFile(const GLchar* file, GLboolean alpha);

	// Loads a single cubemap texture from files
	static TextureCubemap loadTextureCubemapFromFile(const GLchar* path, GLboolean alpha);

	// Loads a single model from file
	static Model* loadModelFromFile(const GLchar* file);

	// Loads a single skinned model from file
	static SkinnedModel* loadSkinnedModelFromFile(const GLchar* file);
};