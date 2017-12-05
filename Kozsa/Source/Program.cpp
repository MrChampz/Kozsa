#pragma once

#include <iostream>

//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Game.h"

// GLFW function declarations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// The Title of the screen
const GLchar* SCREEN_TITLE = "Project Kozsa";

// The Width of the screen
const GLuint SCREEN_WIDTH = 1280;

// The Height of the screen
const GLuint SCREEN_HEIGHT = 720;

Game Kozsa(SCREEN_WIDTH, SCREEN_HEIGHT);

// Camera
GLfloat lastX = 640, lastY = 360;
bool firstMouse = true;

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE, nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();
	glGetError();	// Call it once to catch glewInit() bug, all other errors are now from our application

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLFW configuration
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// OpenGL configuration
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Initialize game
	Kozsa.Init();

	// DeltaTime variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	GLfloat fpsTime = glfwGetTime();
	int count = 0;
	int fps = 0;

	// Start Game within Menu State
	Kozsa.mState = GAME_ACTIVE;

	while (!glfwWindowShouldClose(window))
	{
		// Calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		count++;
		if (currentFrame - fpsTime >= 1.0f)
		{
			fps = count;
			count = 0;
			fpsTime += 1.0f;
		}

		glfwPollEvents();

		// Manage user input
		Kozsa.ProcessInput(deltaTime);

		// Update game state
		Kozsa.Update(deltaTime, fps);

		// Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Kozsa.Render();

		glfwSwapBuffers(window);
	}

	// Shutdown game
	Kozsa.Shutdown();

	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When an user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_LEFT_CONTROL)
	{
		if (action == GLFW_PRESS)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else if (action == GLFW_RELEASE)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			Kozsa.mKeys[key] = GL_TRUE;
		}
		else if (action == GLFW_RELEASE)
		{
			Kozsa.mKeys[key] = GL_FALSE;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button >= 0 && button < 8)
	{
		if (action == GLFW_PRESS)
		{
			Kozsa.mMouseButtons[button] = GL_TRUE;
		}
		else if (action == GLFW_RELEASE)
		{
			Kozsa.mMouseButtons[button] = GL_FALSE;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	Kozsa.mMouseXOffset = xoffset;
	Kozsa.mMouseYOffset = yoffset;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Kozsa.mMouseWheelOffset = yoffset;
}