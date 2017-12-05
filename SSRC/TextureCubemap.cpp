#include "TextureCubemap.h"

TextureCubemap::TextureCubemap()
	: mWidth(0), mHeight(0), mInternalFormat(GL_RGB), mImageFormat(GL_RGB),
	  mWrapS(GL_CLAMP_TO_EDGE), mWrapT(GL_CLAMP_TO_EDGE), mWrapR(GL_CLAMP_TO_EDGE), mFilterMin(GL_LINEAR), mFilterMax(GL_LINEAR), mPath("")
{
	glGenTextures(1, &this->mID);
}

void TextureCubemap::Generate(GLuint width, GLuint height, std::vector<unsigned char*> data)
{
	this->mWidth = width;
	this->mHeight = height;

	// Create texture 
	Bind();
	for (int i = 0; i < data.size(); i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, this->mInternalFormat, width, height, 0, this->mImageFormat, GL_UNSIGNED_BYTE, data[i]);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Set texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, this->mWrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, this->mWrapT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, this->mWrapR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->mFilterMin);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->mFilterMax);

	// Unbind texture
	Unbind();
}

void TextureCubemap::Bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->mID);
}

void TextureCubemap::Unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
