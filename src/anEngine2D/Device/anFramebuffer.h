#ifndef AN_FRAMEBUFFER_H_
#define AN_FRAMEBUFFER_H_

#include "Core/anTypes.h"

struct anFramebufferSpecification
{
	anUInt32 Width;
	anUInt32 Height;
};

class anFramebuffer
{
public:
	anFramebuffer(const anFramebufferSpecification& spec);
	~anFramebuffer();

	void Bind() const;
	void Unbind() const;
	anUInt32 GetRenderbufferID() const;
	anUInt32 GetFramebufferID() const;
	anUInt32 GetTextureID() const;
	const anFramebufferSpecification& GetSpecification() const;
	void Resize(anUInt32 width, anUInt32 height);
private:
	void Prepare();
private:
	anUInt32 mRenderbufferID;
	anUInt32 mFramebufferID;
	anUInt32 mTextureID;

	anFramebufferSpecification mSpec;
};

#endif
