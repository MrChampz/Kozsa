#include "Game.h"

#include <iostream>

#include <STB\stb_image.h>

#include <GLM/glm.hpp>
#include <GLM/gtx/quaternion.hpp>
#include <GLM/common.hpp>

#include "ModelObject.h"
#include "SkyboxObject.h"
#include "Light/PointLight.h"

#include "SkinnedModel.h"

Renderer skyboxRenderer, modelRenderer;
SkyboxObject* skybox;

ModelObject* mannequin;
ModelObject* ground;
SkinnedModel* skinnedModel;
SkinnedModel* skinnedModel1;
SkinnedModel* skinnedModel2;

PointLight pointLights[14];

GLuint envCubemap, irradianceMap, prefilterMap, brdfLUT;
void GenerateCubemaps();
void RenderQuad();
void RenderCube();

Game::Game(GLuint width, GLuint height)
	: mState(GAME_ACTIVE), mKeys(), mMouseXOffset(0), mMouseYOffset(0), mMouseWheelOffset(0), mWidth(width), mHeight(height),
	  mDebugCamera(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Game::~Game()
{
}

std::vector<glm::mat4> frameBones;

void Game::Init()
{
	// Game Init
	ResourceManager::Initialize();

	// Load Shaders
	ResourceManager::LoadShader("Shaders/Skybox.vs", "Shaders/Skybox.fs", nullptr, "Skybox");
	ResourceManager::LoadShader("Shaders/PBR_TEX.vs", "Shaders/PBR_TEX.fs", nullptr, "PBR");
	ResourceManager::LoadShader("Shaders/Skinning.vs", "Shaders/Skinning.fs", nullptr, "Skinning");

	ResourceManager::LoadShader("Shaders/Cubemap.vs", "Shaders/Equirectangular.fs", nullptr, "Equirectangular");
	ResourceManager::LoadShader("Shaders/Cubemap.vs", "Shaders/Irradiance.fs", nullptr, "Irradiance");
	ResourceManager::LoadShader("Shaders/Cubemap.vs", "Shaders/Prefilter.fs", nullptr, "Prefilter");
	ResourceManager::LoadShader("Shaders/BRDF.vs", "Shaders/BRDF.fs", nullptr, "BRDF");

	// Load Textures

	// Load Cubemaps
	ResourceManager::LoadTextureCubemap("Data/Cubemap/", GL_FALSE, "Cubemap");

	// Load Models
	/*
	ResourceManager::LoadModel("Data/Models/Mannequin/Mannequin.pkasset", "Mannequin");
	ResourceManager::LoadModel("Data/Models/Rooms/Rooms.pkasset", "Room");
	ResourceManager::LoadModel("Data/Models/Table/SM_TableRound.pkasset", "Table");
	ResourceManager::LoadModel("Data/Models/Chair/SM_Chair.pkasset", "Chair");
	*/
	ResourceManager::LoadSkinnedModel("Data/Models/Ganfaul/Entry.pkasset", "Ganfaul_Entry");
	ResourceManager::LoadSkinnedModel("Data/Models/Ganfaul/Walking.pkasset", "Ganfaul_Walking");
	ResourceManager::LoadSkinnedModel("Data/Models/Gangnam/Gangnam.pkasset", "Gangnam");
	ResourceManager::LoadModel("Data/Models/Sword/Sword.pkasset", "Sword");
	ResourceManager::LoadModel("Data/Models/Ground/Ground.pkasset", "Ground");

	skyboxRenderer = Renderer(ResourceManager::GetShader("Skybox"));
	modelRenderer = Renderer(ResourceManager::GetShader("PBR"));

	skybox = new SkyboxObject(ResourceManager::GetTextureCubemap("Cubemap"));

	mannequin = new ModelObject(ResourceManager::GetModel("Sword"), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.05f), glm::vec3(0.0f), glm::vec4(1.0f));
	ground = new ModelObject(ResourceManager::GetModel("Ground"), glm::vec3(-5.0f, -0.01f, -5.0f), glm::vec3(0.05f), glm::vec3(0.0f), glm::vec4(1.0f));
	skinnedModel = ResourceManager::GetSkinnedModel("Ganfaul_Entry");
	skinnedModel1 = ResourceManager::GetSkinnedModel("Ganfaul_Walking");
	skinnedModel2 = ResourceManager::GetSkinnedModel("Gangnam");

	ground->mRotation = glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(0.0f), glm::radians(0.0f)));

	pointLights[0] = PointLight(glm::vec3(2.3, 2.3, 2.3), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[1] = PointLight(glm::vec3(2.3, 2.3, 0.0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[2] = PointLight(glm::vec3(0.0, 2.3, 0.0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[3] = PointLight(glm::vec3(0.0, 2.3, 2.3), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[4] = PointLight(glm::vec3(2.3, 0.0, 2.3), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[5] = PointLight(glm::vec3(2.3, 0.0, 0.0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[6] = PointLight(glm::vec3(0.0, 0.0, 0.0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[7] = PointLight(glm::vec3(0.0, 0.0, 2.3), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[8] = PointLight(glm::vec3(1.15, 0.0, 0.0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[9] = PointLight(glm::vec3(1.15, 0.0, 2.3), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[10] = PointLight(glm::vec3(0.0, 0.0, 1.15), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[11] = PointLight(glm::vec3(2.3, 0.0, 1.15), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	pointLights[12] = PointLight(glm::vec3(-1.8f, 0.0, -0.5f), glm::vec4(0.82f, 0.36f, 0.11f, 1.0f));
	pointLights[13] = PointLight(glm::vec3(0.0f, -1.0, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	double animSpeed = 1.0f;
	skinnedModel->SetAnimation("");
	skinnedModel->SetSpeed(animSpeed);
	skinnedModel->Play();

	// Test - Inserting another animation
	Skeleton::Animation walkAnim = skinnedModel1->mSkeleton->mAnimations[""];
	walkAnim.Name = "Walk";
	skinnedModel->mSkeleton->mAnimations["Walk"] = walkAnim;

	skinnedModel1->SetAnimation("");
	skinnedModel1->SetSpeed(animSpeed);
	skinnedModel1->Play();

	skinnedModel2->SetAnimation("");
	skinnedModel2->SetSpeed(animSpeed);
	skinnedModel2->Play();

	frameBones = skinnedModel->mSkeleton->Blend("", "Walk", 40.0f, true);

	GenerateCubemaps();
	glViewport(0, 0, mWidth, mHeight);
}

void Game::Shutdown()
{
	// Delete all resources as loaded using the resource manager
	ResourceManager::Shutdown();
}

void Game::ProcessInput(GLfloat dt)
{
	// Camera Controls
	if (mKeys[GLFW_KEY_W])
		mDebugCamera.ProcessKeyboard(FORWARD, dt);
	if (mKeys[GLFW_KEY_S])
		mDebugCamera.ProcessKeyboard(BACKWARD, dt);
	if (mKeys[GLFW_KEY_A])
		mDebugCamera.ProcessKeyboard(LEFT, dt);
	if (mKeys[GLFW_KEY_D])
		mDebugCamera.ProcessKeyboard(RIGHT, dt);

	if (mKeys[GLFW_KEY_ENTER])
	{
		if (skinnedModel->Paused()) {
			skinnedModel->Play();
		}
		else {
			skinnedModel->Pause();
		}
	}

	if (mKeys[GLFW_KEY_R])
	{
		skinnedModel->Stop();
		skinnedModel->SetAnimation("Walk");
		skinnedModel->Play();
	}
	if (mKeys[GLFW_KEY_T])
	{
		skinnedModel->Stop();
		skinnedModel->SetAnimation("");
		skinnedModel->Play();
	}

	mDebugCamera.ProcessMouseMovement(this->mMouseXOffset, this->mMouseYOffset);
	mDebugCamera.ProcessMouseScroll(this->mMouseWheelOffset);
}

void Game::Update(GLfloat dt, int fps)
{
	this->mMouseXOffset = 0.0f;
	this->mMouseYOffset = 0.0f;
	this->mMouseWheelOffset = 0.0f;

	mannequin->Update(dt);
	ground->Update(dt);
	skinnedModel->Update(dt);
	skinnedModel1->Update(dt);
	skinnedModel2->Update(dt);
}

void Game::Render()
{
	glm::mat4 view, projection;
	glm::vec3 camPos;
	projection = glm::perspective(glm::radians(45.0f), (GLfloat)mWidth / (GLfloat)mHeight, 0.1f, 1000.0f);


	view = mDebugCamera.GetViewMatrix();
	camPos = mDebugCamera.mPosition;

	// Models with PBR Shader
	Shader PbrShader = ResourceManager::GetShader("PBR");
	PbrShader.Use();
	PbrShader.SetMatrix4("view", view);
	PbrShader.SetMatrix4("projection", projection);
	PbrShader.SetVector3f("camPos", camPos);
	for (int i = 0; i < 14; i++)
	{
		PbrShader.SetVector3f(("lightPositions[" + std::to_string(i) + "]").c_str(), pointLights[i].mPosition);
		PbrShader.SetVector4f(("lightColors[" + std::to_string(i) + "]").c_str(), pointLights[i].mColor);
	}
	PbrShader.SetInteger("prefilterMap", 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	PbrShader.SetInteger("irradianceMap", 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	PbrShader.SetInteger("brdfLUT", 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, brdfLUT);

	mannequin->Draw(modelRenderer);
	ground->Draw(modelRenderer);

	// Skinning
	Shader SkinningShader = ResourceManager::GetShader("Skinning");
	SkinningShader.Use();
	SkinningShader.SetMatrix4("view", view);
	SkinningShader.SetMatrix4("projection", projection);
	SkinningShader.SetVector3f("camPos", camPos);
	for (int i = 0; i < 14; i++)
	{
		SkinningShader.SetVector3f(("lightPositions[" + std::to_string(i) + "]").c_str(), pointLights[i].mPosition);
		SkinningShader.SetVector4f(("lightColors[" + std::to_string(i) + "]").c_str(), pointLights[i].mColor);
	}
	SkinningShader.SetInteger("prefilterMap", 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	SkinningShader.SetInteger("irradianceMap", 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	SkinningShader.SetInteger("brdfLUT", 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, brdfLUT);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.002f));
	SkinningShader.SetMatrix4("model", model);
	glUniformMatrix4fv(glGetUniformLocation(SkinningShader.mID, "Bones"), frameBones.size(), GL_FALSE, glm::value_ptr(frameBones[0]));
	skinnedModel->Draw(SkinningShader);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.5f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.002f));
	SkinningShader.SetMatrix4("model", model);
	//skinnedModel1->Draw(SkinningShader);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.002f));
	SkinningShader.SetMatrix4("model", model);
	//skinnedModel2->Draw(SkinningShader);

	// Skybox
	Shader SkyboxShader = ResourceManager::GetShader("Skybox");
	SkyboxShader.Use();
	SkyboxShader.SetMatrix4("view", view);
	SkyboxShader.SetMatrix4("projection", projection);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	skybox->Draw(skyboxRenderer);
}

void GenerateCubemaps()
{
	Shader shaderEquirectangular = ResourceManager::GetShader("Equirectangular");
	Shader shaderIrradiance = ResourceManager::GetShader("Irradiance");
	Shader shaderPrefilter = ResourceManager::GetShader("Prefilter");
	Shader shaderBRDF = ResourceManager::GetShader("BRDF");

	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 views[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GLuint FBO, RBO;
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	GLuint hdrTexture;
	float* data = stbi_loadf("Data/Textures/HDR/GCanyon_C_YumaPoint_3k.hdr", &width, &height, &nrComponents, 0);
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "ERROR::HDR: Failed to load HDR image!" << std::endl;
	}

	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	shaderEquirectangular.Use();
	shaderEquirectangular.SetInteger("equirectangularMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	shaderEquirectangular.SetMatrix4("projection", projection);

	glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (GLuint i = 0; i < 6; ++i)
	{
		shaderEquirectangular.SetMatrix4("view", views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	shaderIrradiance.Use();
	shaderIrradiance.SetInteger("environmentMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	shaderIrradiance.SetMatrix4("projection", projection);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (GLuint i = 0; i < 6; ++i)
	{
		shaderIrradiance.SetMatrix4("view", views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	shaderPrefilter.Use();
	shaderPrefilter.SetInteger("environmentMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	shaderPrefilter.SetMatrix4("projection", projection);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	GLuint maxMipLevels = 5;
	for (GLuint mip = 0; mip < maxMipLevels; ++mip)
	{
		GLuint mipWidth = 128 * std::pow(0.5, mip);
		GLuint mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		shaderPrefilter.SetFloat("roughness", roughness);
		for (GLuint i = 0; i < 6; ++i)
		{
			shaderPrefilter.SetMatrix4("view", views[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &brdfLUT);

	glBindTexture(GL_TEXTURE_2D, brdfLUT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);

	glViewport(0, 0, 512, 512);
	shaderBRDF.Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// RenderCube() Renders a 1x1 3D cube in NDC.
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
void RenderCube()
{
	// Initialize (if necessary)
	if (cubeVAO == 0)
	{
		GLfloat vertices[] = {
			// Back face
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
															  // Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
															   // Left face
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
															  // Right face
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
															 // Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
																// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// Fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// Render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}    