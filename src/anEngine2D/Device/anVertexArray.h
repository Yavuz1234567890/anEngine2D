#ifndef AN_VERTEX_ARRAY_H_
#define AN_VERTEX_ARRAY_H_

#include "Core/anTypes.h"
#include "anBuffers.h"

class anVertexArray
{
public:
	anVertexArray();
	~anVertexArray();

	void AddVertexBuffer(anVertexBuffer* buffer);
	void SetIndexBuffer(anIndexBuffer* buffer);
	void Bind() const;
	void Unbind() const;
	anUInt32 GetID() const;
private:
	anUInt32 mID;
};

#endif
