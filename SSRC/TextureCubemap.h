#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>

/**
 * TextureCubemap is able to store and configure a cubemap texture in OpenGL.
 * It also hosts utility functions for easy management.
 */
class TextureCubemap
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
	GLuint mWrapR;		// Wrapping mode on R axis
	GLuint mFilterMin;	// Filtering mode if texture pixels are smaller than the screen pixels
	GLuint mFilterMax;	// Filtering mode if texture pixels are larger than the screen pixels

	// Texture path
	std::string mPath;

	// Constructor (sets default texture modes)
	TextureCubemap();

	// Generates texture from image data
	void Generate(GLuint width, GLuint height, std::vector<unsigned char*> data);

	// Binds the texture as the current active GL_TEXTURE_CUBE_MAP texture object
	void Bind() const;

	// Unbinds the texture
	void Unbind() const;
};
