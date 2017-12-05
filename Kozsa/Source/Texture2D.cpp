#include "Texture2D.h"

Texture2D::Texture2D()
	: mWidth(0), mHeight(0), mInternalFormat(GL_RGB), mImageFormat(GL_RGB), mWrapS(GL_REPEAT), mWrapT(GL_REPEAT),
	  mFilterMin(GL_LINEAR), mFilterMax(GL_LINEAR), mType(TEXTURE_TYPE_UNDEFINED), mPath("")
{
	glGenTextures(1, &this->mID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
	this->mWidth = width;
	this->mHeight = height;

	// Create texture 
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, this->mInternalFormat, width, height, 0, this->mImageFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->mWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->mWrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->mFilterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mFilterMax);

	// Unbind texture
	Unbind();
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->mID);
}

void Texture2D::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
