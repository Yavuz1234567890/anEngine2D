#include "anFramebuffer.h"
#include "anGPUCommands.h"
#include "Core/anMessage.h"

#include <GL/glew.h>

anFramebuffer::anFramebuffer(const anFramebufferSpecification& spec)
	: mSpec(spec)
{
	Prepare();
}

anFramebuffer::~anFramebuffer()
{
	glDeleteFramebuffers(1, &mFramebufferID);
	glDeleteRenderbuffers(1, &mRenderbufferID);
	glDeleteTextures(1, &mTextureID);
}

void anFramebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
	anSetViewport({ 0.0f, 0.0f }, { (float)mSpec.Width, (float)mSpec.Height });
}

void anFramebuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

anUInt32 anFramebuffer::GetRenderbufferID() const
{
	return mRenderbufferID;
}

anUInt32 anFramebuffer::GetFramebufferID() const
{
	return mFramebufferID;
}

anUInt32 anFramebuffer::GetTextureID() const
{
	return mTextureID;
}

void anFramebuffer::Prepare()
{
	glGenFramebuffers(1, &mFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mSpec.Width, mSpec.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &mRenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mSpec.Width, mSpec.Height);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderbufferID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		anMessage("Error: couldn't created framebuffer!!");

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const anFramebufferSpecification& anFramebuffer::GetSpecification() const
{
	return mSpec;
}

void anFramebuffer::Resize(anUInt32 width, anUInt32 height)
{
	mSpec.Width = width;
	mSpec.Height = height;

	Prepare();
}
