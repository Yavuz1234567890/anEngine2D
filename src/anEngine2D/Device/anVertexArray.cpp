#include "anVertexArray.h"

#include <GL/glew.h>

anVertexArray::anVertexArray()
{
	glGenVertexArrays(1, &mID);
}

anVertexArray::~anVertexArray()
{
	glDeleteVertexArrays(1, &mID);
}

void anVertexArray::AddVertexBuffer(anVertexBuffer* buffer)
{
	glBindVertexArray(mID);
	buffer->Bind();

	int index = 0;
	for (anVertexBufferAttribute attr : buffer->GetAttributes())
	{
		glEnableVertexAttribArray(index);
		switch (attr.Type)
		{
		case anVertexBufferAttribute::Float:
		case anVertexBufferAttribute::Float2:
		case anVertexBufferAttribute::Float3:
		case anVertexBufferAttribute::Float4:
			glVertexAttribPointer(index, attr.Count, GL_FLOAT, attr.Normalized ? GL_TRUE : GL_FALSE, buffer->GetSize(), (const void*)(attr.Offset));
			break;
		case anVertexBufferAttribute::Int:
		case anVertexBufferAttribute::Int2:
		case anVertexBufferAttribute::Int3:
		case anVertexBufferAttribute::Int4:
			glVertexAttribIPointer(index, attr.Count, GL_INT, buffer->GetSize(), (const void*)(attr.Offset));
			break;
		}

		++index;
	}

	buffer->Unbind();
	glBindVertexArray(0);
}

void anVertexArray::SetIndexBuffer(anIndexBuffer* buffer)
{
	glBindVertexArray(mID);
	buffer->Bind();
}

void anVertexArray::Bind() const
{
	glBindVertexArray(mID);
}

void anVertexArray::Unbind() const
{
	glBindVertexArray(0);
}

anUInt32 anVertexArray::GetID() const
{
	return mID;
}
