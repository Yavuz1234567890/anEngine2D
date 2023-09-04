#ifndef AN_GPU_COMMANDS_H_
#define AN_GPU_COMMANDS_H_

#include "Core/anTypes.h"
#include "anVertexArray.h"
#include "Math/anFloat2.h"

namespace anDrawType
{
	enum : anUInt32
	{
		Triangles,
		Lines
	};
}

void anClear();
void anClearColor(const anColor& color);
void anSetViewport(const anFloat2& pos, const anFloat2& size);
void anDrawIndexed(anUInt32 type, anUInt32 count, anVertexArray* vertexArray);
void anDrawArrays(anUInt32 type, anUInt32 size, anVertexArray* vertexArray);
void anPixelStore();
void anEnableBlend();
void anDisableBlend();
anFloat2 anGetViewportPosition();
anFloat2 anGetViewportSize();
anColor anGetClearColor();

#endif
