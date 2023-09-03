#include "anBuffers.h"

#include <GL/glew.h>

static GLenum anBufferUsageToOpenGL(anUInt32 usage)
{
	switch (usage)
	{
	case anBufferUsage::Dynamic:	return GL_DYNAMIC_DRAW;
	case anBufferUsage::Static:		return GL_STATIC_DRAW;
	}

	return 0;
}

static anUInt32 anGetAttributeCount(anUInt32 type)
{
	switch (type)
	{
	case anVertexBufferAttribute::Float:	return 1;
	case anVertexBufferAttribute::Float2:	return 2;
	case anVertexBufferAttribute::Float3:	return 3;
	case anVertexBufferAttribute::Float4:	return 4;
	case anVertexBufferAttribute::Int:		return 1;
	case anVertexBufferAttribute::Int2:		return 2;
	case anVertexBufferAttribute::Int3:		return 3;
	case anVertexBufferAttribute::Int4:		return 4;
	}

	return -1;
}

static anUInt32 anGetAttributeSize(anUInt32 type)
{
	switch (type)
	{
	case anVertexBufferAttribute::Float:
	case anVertexBufferAttribute::Float2:
	case anVertexBufferAttribute::Float3:
	case anVertexBufferAttribute::Float4:
		return sizeof(float);
	case anVertexBufferAttribute::Int:
	case anVertexBufferAttribute::Int2:
	case anVertexBufferAttribute::Int3:
	case anVertexBufferAttribute::Int4:
		return sizeof(int);
	}

	return 0;
}

anVertexBuffer::anVertexBuffer(anUInt32 size)
	: mStride(0)
{
	mUsage = anBufferUsage::Dynamic;

	glGenBuffers(1, &mID);
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

anVertexBuffer::anVertexBuffer(const void* data, anUInt32 size)
	: mStride(0)
{
	mUsage = anBufferUsage::Static;

	glGenBuffers(1, &mID);
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

anVertexBuffer::~anVertexBuffer()
{
	glDeleteBuffers(1, &mID);
}

void anVertexBuffer::Resize(anUInt32 size)
{
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, anBufferUsageToOpenGL(mUsage));
}

void anVertexBuffer::SetData(const void* data, anUInt32 size)
{
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void anVertexBuffer::PushAttribute(anUInt32 type, bool normalized)
{
	anUInt32 size = anGetAttributeSize(type);
	anUInt32 count = anGetAttributeCount(type);

	anVertexBufferAttribute attr;
	attr.Type = type;
	attr.Normalized = normalized;
	attr.Offset = mStride;
	attr.Count = count;
	mAttributes.push_back(attr);
	
	mStride += count * size;
}

void anVertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, mID);
}

void anVertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

anUInt32 anVertexBuffer::GetID() const
{
	return mID;
}

const anVector<anVertexBufferAttribute>& anVertexBuffer::GetAttributes() const
{
	return mAttributes;
}

anUInt32 anVertexBuffer::GetSize() const
{
	return mStride;
}

anIndexBuffer::anIndexBuffer(anUInt64 size)
{
	mUsage = anBufferUsage::Dynamic;

	glGenBuffers(1, &mID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

anIndexBuffer::anIndexBuffer(const void* indices, anUInt64 size)
{
	mUsage = anBufferUsage::Static;

	glGenBuffers(1, &mID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (const void*)indices, GL_STATIC_DRAW);
}

anIndexBuffer::~anIndexBuffer()
{
	glDeleteBuffers(1, &mID);
}

void anIndexBuffer::Resize(anUInt32 size)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, anBufferUsageToOpenGL(mUsage));
}

void anIndexBuffer::SetData(const void* data, anUInt32 size)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
}

void anIndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
}

void anIndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

anUInt32 anIndexBuffer::GetID() const
{
	return mID;
}
