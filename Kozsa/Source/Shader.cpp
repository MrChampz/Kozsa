﻿#include "Shader.h"

#include <iostream>

Shader::Shader()
{
}

Shader& Shader::Use()
{
	glUseProgram(this->mID);
	return *this;
}

void Shader::Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource)
{
	GLuint sVertex, sFragment, sGeometry;

	// Vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");

	// Fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");

	// If Geometry Shader source code is given, also compile geometry shader
	if (geometrySource != nullptr)
	{
		sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(sGeometry, 1, &geometrySource, NULL);
		glCompileShader(sGeometry);
		checkCompileErrors(sGeometry, "GEOMETRY");
	}

	// Shader Program
	this->mID = glCreateProgram();
	glAttachShader(this->mID, sVertex);
	glAttachShader(this->mID, sFragment);
	if (geometrySource != nullptr)
		glAttachShader(this->mID, sGeometry);
	glLinkProgram(this->mID);
	checkCompileErrors(this->mID, "PROGRAM");

	// Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr)
		glDeleteShader(sGeometry);

}

void Shader::SetFloat(const GLchar* name, GLfloat value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform1f(glGetUniformLocation(this->mID, name), value);
}

void Shader::SetInteger(const GLchar* name, GLint value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform1i(glGetUniformLocation(this->mID, name), value);
}

void Shader::SetVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->mID, name), x, y);
}

void Shader::SetVector2f(const GLchar* name, glm::vec2& value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->mID, name), value.x, value.y);
}

void Shader::SetVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->mID, name), x, y, z);
}

void Shader::SetVector3f(const GLchar* name, glm::vec3& value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->mID, name), value.x, value.y, value.z);
}

void Shader::SetVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->mID, name), x, y, z, w);
}

void Shader::SetVector4f(const GLchar* name, glm::vec4& value, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->mID, name), value.x, value.y, value.z, value.w);
}

void Shader::SetMatrix4(const GLchar* name, glm::mat4& matrix, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniformMatrix4fv(glGetUniformLocation(this->mID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(GLuint object, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
					  << infoLog << "\n -- --------------------------------------------------- -- "
					  << std::endl;
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}
