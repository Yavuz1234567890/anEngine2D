#include "anTexture.h"
#include "Core/anMessage.h"

#include <GL/glew.h>
#include <stb_image.h>

static anTexture* sWhiteTexture = nullptr;
static bool sWhiteTextureCreated = false;

void anTexture::Initialize()
{
	if (!sWhiteTextureCreated)
	{
		anUInt32 whitePixel = 0xffffffff;
		sWhiteTexture = new anTexture(&whitePixel, 1, 1);

		sWhiteTextureCreated = true;
	}
}

anTexture::anTexture(const void* data, anUInt32 width, anUInt32 height, anUInt32 iformat, anUInt32 dformat)
{
	mWidth = width;
	mHeight = height;
	mInternalFormat = iformat;
	mDataFormat = dformat;

	GLenum internalFormat = 0;
	GLenum dataFormat = 0;

	if (iformat == anTextureFormat::RGB)
		internalFormat = GL_RGB8;
	if (iformat == anTextureFormat::RGBA)
		internalFormat = GL_RGBA8;
	if (iformat == anTextureFormat::Red)
		internalFormat = GL_RED;
	
	if (dformat == anTextureFormat::RGB)
		dataFormat = GL_RGB;
	if (dformat == anTextureFormat::RGBA)
		dataFormat = GL_RGBA;
	if (dformat == anTextureFormat::Red)
		dataFormat = GL_RED;
	
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

anTexture::~anTexture()
{
	glDeleteTextures(1, &mID);
}

void anTexture::Bind(anUInt32 slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, mID);
}

void anTexture::Unbind(anUInt32 slot) const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

anUInt32 anTexture::GetID() const
{
	return mID;
}

anUInt32 anTexture::GetWidth() const
{
	return mWidth;
}

anUInt32 anTexture::GetHeight() const
{
	return mHeight;
}

anUInt32 anTexture::GetInternalFormat() const
{
	return mInternalFormat;
}

anUInt32 anTexture::GetDataFormat() const
{
	return mDataFormat;
}

anTexture* anTexture::GetWhiteTexture()
{
	return sWhiteTexture;
}

anTexture* anLoadTexture(const anString& path)
{
	int channels = 0;
	int width = 0;
	int height = 0;

	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
	if (!data)
		anShowMessageBox("Couldn't find texture '" + path + "'");

	anUInt32 internalFormat = 0;
	anUInt32 dataFormat = 0;
	if (channels == 4)
	{
		internalFormat = anTextureFormat::RGBA;
		dataFormat = anTextureFormat::RGBA;
	}

	if (channels == 3)
	{
		internalFormat = anTextureFormat::RGB;
		dataFormat = anTextureFormat::RGB;
	}

	anTexture* texture = new anTexture(data, width, height, internalFormat, dataFormat);
	
	stbi_image_free(data);
	
	return texture;
}
