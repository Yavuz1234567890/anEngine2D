#include "anRenderer.h"
#include "Device/anShaders.h"
#include "Core/anMessage.h"
#include "Device/anGPUCommands.h"

anRenderer::anRenderer()
	: mLineVertexArray(nullptr)
	, mLineVertexBuffer(nullptr)
	, mTextureVertexArray(nullptr)
	, mTextureIndexBuffer(nullptr)
	, mTextureVertexBuffer(nullptr)
{
	for (anUInt32 i = 0; i < anMaxTextureSlots; i++)
		mTextures[i] = nullptr;
}

anRenderer::~anRenderer()
{
}

void anRenderer::Initialize()
{
	mLineVertexArray = new anVertexArray();

	mLineVertexBuffer = new anVertexBuffer(anMaxVertices * sizeof(anColorVertex));
	mLineVertexBuffer->PushAttribute(anVertexBufferAttribute::Float3);
	mLineVertexBuffer->PushAttribute(anVertexBufferAttribute::Float4);

	mLineVertexArray->AddVertexBuffer(mLineVertexBuffer);

	mQuadPositions[0] = { -0.5f, -0.5f, 0.0f };
	mQuadPositions[1] = {  0.5f, -0.5f, 0.0f };
	mQuadPositions[2] = {  0.5f,  0.5f, 0.0f };
	mQuadPositions[3] = { -0.5f,  0.5f, 0.0f };

	mQuadTexCoords[0] = { 0.0f, 0.0f };
	mQuadTexCoords[1] = { 1.0f, 0.0f };
	mQuadTexCoords[2] = { 1.0f, 1.0f };
	mQuadTexCoords[3] = { 0.0f, 1.0f };

	mTextureVertexArray = new anVertexArray();
	
	mTextureVertexBuffer = new anVertexBuffer(anMaxVertices * sizeof(anTextureVertex));
	mTextureVertexBuffer->PushAttribute(anVertexBufferAttribute::Float3);
	mTextureVertexBuffer->PushAttribute(anVertexBufferAttribute::Float2);
	mTextureVertexBuffer->PushAttribute(anVertexBufferAttribute::Float4);
	mTextureVertexBuffer->PushAttribute(anVertexBufferAttribute::Int);

	mTextureVertexArray->AddVertexBuffer(mTextureVertexBuffer);

	anUInt32* quadIndices = new anUInt32[anMaxIndices];
	for (anUInt32 i = 0; i < anMaxQuads; i++)
	{
		quadIndices[i * 6 + 0] = i * 4 + 0;
		quadIndices[i * 6 + 1] = i * 4 + 1;
		quadIndices[i * 6 + 2] = i * 4 + 2;
					 	
		quadIndices[i * 6 + 3] = i * 4 + 2;
		quadIndices[i * 6 + 4] = i * 4 + 3;
		quadIndices[i * 6 + 5] = i * 4 + 0;
	}

	mTextureIndexBuffer = new anIndexBuffer(quadIndices, anMaxIndices);

	mTextureVertexArray->SetIndexBuffer(mTextureIndexBuffer);

	mTextures[anWhiteTextureSlot] = anTexture::GetWhiteTexture();
	mTextureIndex = 1;

	int samplers[anMaxTextureSlots];
	samplers[anWhiteTextureSlot] = anWhiteTextureSlot;
	for (int i = 1; i < anMaxTextureSlots; i++)
		samplers[i] = i;

	anGetTextureShader()->Bind();
	anGetTextureShader()->SetUniformIntArray("uSamplers", anMaxTextureSlots, samplers);
	anGetTextureShader()->Unbind();
}

void anRenderer::SetMatrix(const anMatrix4& matrix)
{
	mMatrix = matrix;
}

void anRenderer::Flush()
{
	if (!mLineVertices.empty())
	{
		mLineVertexBuffer->SetData(mLineVertices.data(), mLineVertices.size() * sizeof(anColorVertex));
	
		anGetColorShader()->Bind();
		anGetColorShader()->SetUniformMatrix4("uMatrix", mMatrix);
	
		anDrawArrays(anDrawType::Lines, mLineVertices.size(), mLineVertexArray);
	
		anGetColorShader()->Unbind();
	
		mLineVertices.clear();
		mLineVertexCount = 0;
	}

	if (!mTextureVertices.empty())
	{
		mTextureVertexBuffer->SetData(mTextureVertices.data(), mTextureVertices.size() * sizeof(anTextureVertex));

		for (anUInt32 i = 0; i < mTextureIndex; i++)
			mTextures[i]->Bind(i);

		anGetTextureShader()->Bind();
		anGetTextureShader()->SetUniformMatrix4("uMatrix", mMatrix);

		anDrawIndexed(anDrawType::Triangles, mTextureIndexCount, mTextureVertexArray);
		
		anGetTextureShader()->Unbind();

		mTextureVertices.clear();
		mTextureIndex = 1;
		mTextureIndexCount = 0;
	}
}

void anRenderer::Start()
{
}

void anRenderer::End()
{
	Flush();
}

void anRenderer::StartDraw()
{
	if (mLineVertexCount >= anMaxVertices || mTextureIndexCount >= anMaxIndices || mTextureIndex >= anMaxTextureSlots)
		Flush();
}

void anRenderer::DrawLine(const anFloat2& start, const anFloat2& end, const anColor& color)
{
	StartDraw();

	anColorVertex v0;
	v0.Position = { start.X, start.Y, 0.0f };
	v0.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	mLineVertices.push_back(v0);

	anColorVertex v1;
	v1.Position = { end.X, end.Y, 0.0f };
	v1.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	mLineVertices.push_back(v1);

	mLineVertexCount += 2;
}

void anRenderer::DrawQuad(const anFloat2& pos, const anFloat2& size, const anColor& color)
{
	anFloat2 vertices[4];
	for (anUInt32 i = 0; i < 4; i++)
		vertices[i] = { pos.X + mQuadPositions[i].X * size.X, pos.Y + mQuadPositions[i].Y * size.Y };

	DrawLineVertices(vertices, 4, color);
}

void anRenderer::DrawQuad(const anFloat2& pos, const anFloat2& size, float rot, const anColor& color)
{
	anMatrix4 transform = 
		  anMatrix4::Scale({ size.X, size.Y, 0.0f })
		* anMatrix4::Translate({ pos.X, pos.Y, 0.0f, })
		* anMatrix4::Rotate(rot, { 0.0f, 0.0f, 1.0f });

	anFloat2 vertices[4];
	for (anUInt32 i = 0; i < 4; i++)
	{
		anFloat3 vert = mQuadPositions[i] * transform;

		vertices[i].X = vert.X;
		vertices[i].Y = vert.Y;
	}

	DrawLineVertices(vertices, 4, color);
}

void anRenderer::DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anColor& color)
{
	StartDraw();

	anUInt32 index = GetTextureIndex(texture);

	for (anUInt32 i = 0; i < 4; i++)
	{
		anTextureVertex vertex;
		vertex.Position = { pos.X + mQuadPositions[i].X * size.X, pos.Y + mQuadPositions[i].Y * size.Y, 0.0f };
		vertex.TexCoord = mQuadTexCoords[i];
		vertex.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		vertex.TexIndex = (int)index;
		mTextureVertices.push_back(vertex);
	}

	mTextureIndexCount += 6;
}

void anRenderer::DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, float rot, const anColor& color)
{
	StartDraw();

	anMatrix4 transform =
		anMatrix4::Translate({ pos.X, pos.Y, 0.0f, })
		* anMatrix4::Scale({ size.X, size.Y, 0.0f })
		* anMatrix4::Rotate(rot, { 0.0f, 0.0f, 1.0f });

	anUInt32 index = GetTextureIndex(texture);
	for (anUInt32 i = 0; i < 4; i++)
	{
		anTextureVertex vertex;
		vertex.Position = mQuadPositions[i] * transform;
		vertex.TexCoord = mQuadTexCoords[i];
		vertex.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		vertex.TexIndex = (int)index;
		mTextureVertices.push_back(vertex);
	}

	mTextureIndexCount += 6;
}

void anRenderer::DrawLineVertices(anFloat2* vertices, anUInt32 size, const anColor& color)
{
	for (anUInt32 i = 0; i < size - 1; i++)
		DrawLine(vertices[i], vertices[i + 1], color);

	DrawLine(vertices[size - 1], vertices[0], color);
}

anUInt32 anRenderer::GetTextureIndex(anTexture* texture)
{
	if (texture)
	{
		for (anUInt32 i = 0; i < mTextureIndex; i++)
		{
			if (mTextures[i]->GetID() == texture->GetID())
				return i;
		}

		mTextures[mTextureIndex] = texture;
		++mTextureIndex;
		return mTextureIndex - 1;
	}

	return 0;
}
