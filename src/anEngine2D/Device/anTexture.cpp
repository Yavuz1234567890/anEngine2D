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
		anTextureCreationSpecification whiteTextureSpec;
		whiteTextureSpec.Data = &whitePixel;
		whiteTextureSpec.Width = 1;
		whiteTextureSpec.Height = 1;
		whiteTextureSpec.Format = anTextureFormat::RGBA;
		whiteTextureSpec.MinFilter = anTextureParameter::Nearest;
		whiteTextureSpec.MagFilter = anTextureParameter::Nearest;
		whiteTextureSpec.WrapS = anTextureParameter::Repeat;
		whiteTextureSpec.WrapT = anTextureParameter::Repeat;

		sWhiteTexture = new anTexture(whiteTextureSpec);

		sWhiteTextureCreated = true;
	}
}

template<typename T>
static T _anGetTextureParameter(anUInt32 type)
{
	switch (type)
	{
	case anTextureParameter::ClampToEdge: return GL_CLAMP_TO_EDGE;
	case anTextureParameter::Linear: return GL_LINEAR;
	case anTextureParameter::Nearest: return GL_NEAREST;
	case anTextureParameter::Repeat: return GL_REPEAT;
	case anTextureParameter::Red: return GL_RED;
	case anTextureParameter::Green: return GL_GREEN;
	case anTextureParameter::Blue: return GL_BLUE;
	case anTextureParameter::One: return GL_ONE;
	case anTextureParameter::Zero: return GL_ZERO;
	case anTextureParameter::Alpha: return GL_ALPHA;
	}

	return 0;
}

anTexture::anTexture(const anTextureCreationSpecification& spec)
{
	mSpecification = spec;
	
	GLenum internalFormat = 0;
	GLenum dataFormat = 0;

	if (spec.Format == anTextureFormat::RGB)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	if (spec.Format == anTextureFormat::RGBA)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}

	if (spec.Format == anTextureFormat::Red)
	{
		internalFormat = GL_R8;
		dataFormat = GL_RED;
	}

	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _anGetTextureParameter<float>(spec.MinFilter));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _anGetTextureParameter<float>(spec.MagFilter));
	
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _anGetTextureParameter<int>(spec.WrapS));
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _anGetTextureParameter<int>(spec.WrapT));

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, spec.Width, spec.Height, 0, dataFormat, GL_UNSIGNED_BYTE, spec.Data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

anTexture::~anTexture()
{
	glDeleteTextures(1, &mID);
}

void anTexture::SetTextureSwizzle(anUInt32 r, anUInt32 g, anUInt32 b, anUInt32 a)
{
	glBindTexture(GL_TEXTURE_2D, mID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, _anGetTextureParameter<int>(r));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, _anGetTextureParameter<int>(g));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, _anGetTextureParameter<int>(b));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, _anGetTextureParameter<int>(a));

	glBindTexture(GL_TEXTURE_2D, 0);
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
	return mSpecification.Width;
}

anUInt32 anTexture::GetHeight() const
{
	return mSpecification.Height;
}

anUInt32 anTexture::GetFormat() const
{
	return mSpecification.Format;
}

const void* anTexture::GetData() const
{
	return mSpecification.Data;
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

	anUInt32 format = 0;
	if (channels == 4)
		format = anTextureFormat::RGBA;
	if (channels == 3)
		format = anTextureFormat::RGB;
	if (channels == 2)
		format = anTextureFormat::Red;

	anTextureCreationSpecification spec;
	spec.Data = data;
	spec.Width = width;
	spec.Height = height;
	spec.Format = format;
	spec.MinFilter = anTextureParameter::Nearest;
	spec.MagFilter = anTextureParameter::Nearest;
	spec.WrapS = anTextureParameter::Repeat;
	spec.WrapT = anTextureParameter::Repeat;
	anTexture* texture = new anTexture(spec);
	
	stbi_image_free(data);
	
	return texture;
}
