#include "anGPUCommands.h"

#include <GL/glew.h>

static GLenum anDrawTypeToOpenGLMode(anUInt32 type)
{
	switch (type)
	{
	case anDrawType::Triangles: return GL_TRIANGLES;
	case anDrawType::Lines: return GL_LINES;
	}

	return 0;
}

static struct
{
	anColor ClearColor;

	anFloat2 ViewportPosition;
	anFloat2 ViewportSize;
} sDeviceData;

void anClear()
{
	sDeviceData.ClearColor = { 0, 0, 0, 255 };

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void anClearColor(const anColor& color)
{
	sDeviceData.ClearColor = color;

	glClearColor((float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void anSetViewport(const anFloat2& pos, const anFloat2& size)
{
	sDeviceData.ViewportPosition = pos;
	sDeviceData.ViewportSize = size;

	glViewport((GLsizei)pos.x, (GLsizei)pos.y, (GLsizei)size.x, (GLsizei)size.y);
}

void anDrawIndexed(anUInt32 type, anUInt32 count, anVertexArray* vertexArray)
{
	vertexArray->Bind();
	glDrawElements(anDrawTypeToOpenGLMode(type), count, GL_UNSIGNED_INT, nullptr);
	vertexArray->Unbind();
}

void anDrawArrays(anUInt32 type, anUInt32 size, anVertexArray* vertexArray)
{
	vertexArray->Bind();
	glDrawArrays(anDrawTypeToOpenGLMode(type), 0, size);
	vertexArray->Unbind();
}

void anPixelStore()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void anEnableBlend()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void anDisableBlend()
{
	glDisable(GL_BLEND);
}

anFloat2 anGetViewportPosition()
{
	return sDeviceData.ViewportPosition;
}

anFloat2 anGetViewportSize()
{
	return sDeviceData.ViewportSize;
}

anColor anGetClearColor()
{
	return sDeviceData.ClearColor;
}
