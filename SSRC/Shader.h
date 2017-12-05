#pragma once

#include <string>

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

/**
 * General purpose shader object. Compiles from file, generates
 * compile/link-time error messages and hosts several utility
 * functions for easy management.
 */
class Shader
{
public:
	// State
	GLuint mID;

public:
	// Constructor
	Shader();

	// Sets the current shader as active
	Shader& Use();

	// Compiles the shader from given source code
	// Note: geometry shader source code is optional
	void Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource = nullptr);

	// Utility functions
	void SetFloat		(const GLchar* name, GLfloat value, GLboolean useShader = GL_FALSE);
	void SetInteger		(const GLchar* name, GLint value, GLboolean useShader = GL_FALSE);
	void SetVector2f	(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader = GL_FALSE);
	void SetVector2f	(const GLchar* name, glm::vec2& value, GLboolean useShader = GL_FALSE);
	void SetVector3f	(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = GL_FALSE);
	void SetVector3f	(const GLchar* name, glm::vec3& value, GLboolean useShader = GL_FALSE);
	void SetVector4f	(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = GL_FALSE);
	void SetVector4f	(const GLchar* name, glm::vec4& value, GLboolean useShader = GL_FALSE);
	void SetMatrix4		(const GLchar* name, glm::mat4& matrix, GLboolean useShader = GL_FALSE);

private:
	// Checks if compilation or linking failed and if so, print the error logs
	void checkCompileErrors(GLuint object, std::string type);
};
