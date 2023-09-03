#ifndef AN_BUFFERS_H_
#define AN_BUFFERS_H_

#include "Core/anTypes.h"

namespace anBufferUsage
{
	enum : anUInt32
	{
		Static,
		Dynamic
	};
}

struct anVertexBufferAttribute
{
	enum : anUInt32
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4
	};

	anUInt32 Type;
	bool Normalized;
	anUInt32 Offset;
	anUInt32 Count;
};

class anVertexBuffer
{
public:
	anVertexBuffer(anUInt32 size);
	anVertexBuffer(const void* data, anUInt32 size);
	~anVertexBuffer();

	void Resize(anUInt32 size);
	void SetData(const void* data, anUInt32 size);
	void PushAttribute(anUInt32 type, bool normalized = false);
	void Bind() const;
	void Unbind() const;
	anUInt32 GetID() const;
	const anVector<anVertexBufferAttribute>& GetAttributes() const;
	anUInt32 GetSize() const;
private:
	anUInt32 mID;
	anVector<anVertexBufferAttribute> mAttributes;
	anUInt32 mStride;
	anUInt32 mUsage;
};

class anIndexBuffer
{
public:
	anIndexBuffer(anUInt64 size);
	anIndexBuffer(const void* indices, anUInt64 size);
	~anIndexBuffer();

	void Resize(anUInt32 size);
	void SetData(const void* data, anUInt32 size);
	void Bind() const;
	void Unbind() const;
	anUInt32 GetID() const;
private:
	anUInt32 mID;
	anUInt32 mUsage;
};

#endif
