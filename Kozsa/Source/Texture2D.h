#pragma once

#include <string>

#include <GL/glew.h>

// Represent a texture type
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

/**
 * Texture2D is able to store and configure a texture in OpenGL.
 * It also hosts utility functions for easy management.
 */
class Texture2D
{
public:
	// Holds the Id of the texture object, used for all texture operations to reference to this particular texture
	GLuint mID;

	// Texture image dimensions
	// Width and Height of loaded image in pixels
	GLuint mWidth, mHeight;

	// Texture format
	GLuint mInternalFormat;	// Format of texture object
	GLuint mImageFormat;		// Format of loaded image

	// Texture configuration
	GLuint mWrapS;		// Wrapping mode on S axis
	GLuint mWrapT;		// Wrapping mode on T axis
	GLuint mFilterMin;	// Filtering mode if texture pixels are smaller than the screen pixels
	GLuint mFilterMax;	// Filtering mode if texture pixels are larger than the screen pixels

	// Texture type
	TextureType mType;

	// Texture path
	std::string mPath;

	// Constructor (sets default texture modes)
	Texture2D();

	// Generates texture from image data
	void Generate(GLuint width, GLuint height, unsigned char* data);

	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;

	// Unbinds the texture
	void Unbind() const;
};
