#ifndef AN_TEXTURE_H_
#define AN_TEXTURE_H_

#include "Core/anTypes.h"

namespace anTextureFormat
{
	enum : anUInt32
	{
		RGB,
		RGBA,
		Red
	};
}

namespace anTextureParameter
{
	enum : anUInt32
	{
		Nearest,
		Linear,
		Repeat,
		ClampToEdge,
		Red,
		Green,
		Blue,
		One,
		Zero,
		Alpha
	};
}

struct anTextureCreationSpecification
{
	const void* Data;
	anUInt32 Width;
	anUInt32 Height;
	anUInt32 Format;
	anUInt32 MinFilter;
	anUInt32 MagFilter;
	anUInt32 WrapS;
	anUInt32 WrapT;
};

class anTexture
{
public:
	anTexture(const anTextureCreationSpecification& spec);
	~anTexture();

	void SetTextureSwizzle(anUInt32 r, anUInt32 g, anUInt32 b, anUInt32 a);
	void Bind(anUInt32 slot = 0) const;
	void Unbind(anUInt32 slot = 0) const;
	anUInt32 GetID() const;
	anUInt32 GetWidth() const;
	anUInt32 GetHeight() const;
	anUInt32 GetFormat() const;
	const void* GetData() const;
public:
	static void Initialize();
	static anTexture* GetWhiteTexture();
private:
	anUInt32 mID;
	anTextureCreationSpecification mSpecification;
};

anTexture* anLoadTexture(const anString& path);

#endif
