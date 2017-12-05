#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ModelObject.h"
#include "Camera.h"
#include "ResourceManager.h"

// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU
};

/**
 * Game holds all game-related state and functionality.
 * Combines all game-related data into a single class for
 * easy access to each of the components and manageability.
 */
class Game
{
public:
	// Game state
	GameState	mState;
	GLboolean	mKeys[1024];
	GLboolean	mMouseButtons[8];
	GLfloat		mMouseXOffset, mMouseYOffset, mMouseWheelOffset;
	GLuint		mWidth, mHeight;

	// State
	Camera				mDebugCamera;
	
public:
	// Constructor/Destructor
	Game(GLuint width, GLuint height);
	~Game();

	// Initialize game state (load all shaders/textures/models/maps)
	void Init();

	// Shutdown game state
	void Shutdown();

	// Game Loop
	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt, int fps);
	void Render();
};
