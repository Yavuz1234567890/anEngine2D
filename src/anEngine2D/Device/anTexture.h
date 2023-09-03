#ifndef AN_TEXTURE_H_
#define AN_TEXTURE_H_

#include "Core/anTypes.h"

namespace anTextureFormat
{
	enum : anUInt32
	{
		RGB,
		RGBA
	};
}

class anTexture
{
public:
	anTexture(const void* data, anUInt32 width, anUInt32 height, anUInt32 iformat = anTextureFormat::RGBA, anUInt32 dformat = anTextureFormat::RGBA);
	~anTexture();

	void Bind(anUInt32 slot = 0) const;
	void Unbind(anUInt32 slot = 0) const;
	anUInt32 GetID() const;
	anUInt32 GetWidth() const;
	anUInt32 GetHeight() const;
	anUInt32 GetInternalFormat() const;
	anUInt32 GetDataFormat() const;
public:
	static void Initialize();
	static anTexture* GetWhiteTexture();
private:
	anUInt32 mID;
	anUInt32 mWidth;
	anUInt32 mHeight;
	anUInt32 mInternalFormat;
	anUInt32 mDataFormat;
};

anTexture* anLoadTexture(const anString& path);

#endif
