#include "anRenderer.h"
#include "Device/anShaders.h"
#include "Core/anMessage.h"
#include "Device/anGPUCommands.h"

anRenderer::anRenderer()
{
	for (anUInt32 i = 0; i < anMaxTextureSlots; i++)
		mTextures[i] = nullptr;
}

anRenderer::~anRenderer()
{
}

void anRenderer::Initialize()
{
	mShader = anGetBatchShader();

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

	mShader->Bind();
	mShader->SetUniformIntArray("uSamplers", anMaxTextureSlots, samplers);
	mShader->Unbind();
}

void anRenderer::SetMatrix(const anMatrix4& matrix)
{
	mMatrix = matrix;
}

void anRenderer::Flush()
{
	if (!mTextureVertices.empty())
	{
		mTextureVertexBuffer->SetData(mTextureVertices.data(), mTextureVertices.size() * sizeof(anTextureVertex));

		for (anUInt32 i = 0; i < mTextureIndex; i++)
			mTextures[i]->Bind(i);

		mShader->Bind();
		mShader->SetUniformMatrix4("uMatrix", mMatrix);

		anDrawIndexed(anDrawType::Triangles, mTextureIndexCount, mTextureVertexArray);
		
		mShader->Unbind();

		mTextureVertices.clear();
		mTextureIndex = 1;
		mTextureIndexCount = 0;

		++mDrawCallCount;
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
	if (mTextureIndexCount >= anMaxIndices || mTextureIndex >= anMaxTextureSlots)
		Flush();
}

void anRenderer::DrawLine(const anFloat2& start, const anFloat2& end, const anColor& color, float width)
{
	StartDraw();

	anTextureVertex v0;
	v0.Position = { start.X, start.Y, 0.0f };
	v0.TexCoord = mQuadTexCoords[0];
	v0.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	v0.TexIndex = anWhiteTextureSlot;
	mTextureVertices.push_back(v0);

	anTextureVertex v1;
	v1.Position = { start.X + width, start.Y, 0.0f };
	v1.TexCoord = mQuadTexCoords[1];
	v1.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	v1.TexIndex = anWhiteTextureSlot;
	mTextureVertices.push_back(v1);

	anTextureVertex v2;
	v2.Position = { end.X + width, end.Y + width, 0.0f };
	v2.TexCoord = mQuadTexCoords[2];
	v2.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	v2.TexIndex = anWhiteTextureSlot;
	mTextureVertices.push_back(v2);

	anTextureVertex v3;
	v3.Position = { end.X, end.Y + width, 0.0f };
	v3.TexCoord = mQuadTexCoords[3];
	v3.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	v3.TexIndex = anWhiteTextureSlot;
	mTextureVertices.push_back(v3);

	mTextureIndexCount += 6;
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

void anRenderer::DrawTextureSub(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anFloat2& spos, const anFloat2& ssize, float rot, const anColor& color)
{
	StartDraw();

	const int texWidth = (int)texture->GetWidth();
	const int texHeight = (int)texture->GetHeight();

	anMatrix4 transform =
		anMatrix4::Translate({ pos.X, pos.Y, 0.0f, })
		* anMatrix4::Scale({ size.X, size.Y, 0.0f })
		* anMatrix4::Rotate(rot, { 0.0f, 0.0f, 1.0f });

	anUInt32 index = GetTextureIndex(texture);
	for (anUInt32 i = 0; i < 4; i++)
	{
		anTextureVertex vertex;
		vertex.Position = mQuadPositions[i] * transform;
		vertex.TexCoord = { (mQuadTexCoords[i].X * ssize.X + spos.X) / texWidth, (mQuadTexCoords[i].Y * ssize.Y + spos.Y) / texHeight };
		vertex.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		vertex.TexIndex = (int)index;
		mTextureVertices.push_back(vertex);
	}

	mTextureIndexCount += 6;
}

void anRenderer::DrawString(const anFont& font, const anFloat2& pos, const anString& str, const anColor& color)
{
	anUInt32 spaceAdvance = font.GetCharacter(' ').Advance;
	anUInt32 tabAdvance = spaceAdvance * 4;
    
	float x = pos.X;
	float y = pos.Y;
	for (anUInt32 i = 0; i < str.size(); i++)
	{
		if (str[i] == ' ')
		{
			x += spaceAdvance >> 6;
			continue;
		}

		if (str[i] == '\t')
		{
			x += tabAdvance >> 6;
			continue;
		}

		if (str[i] == '\r')
			continue;

		if (str[i] == '\n')
		{
			x = pos.X;
			// for now
			y += (float)font.GetSize();
			continue;
		}

		StartDraw();

		anCharacter ch = font.GetCharacter(str[i]);

		float w = ch.Size.X;
		float h = ch.Size.Y;

		anUInt32 texIndex = GetTextureIndex(ch.Texture);

		float xpos = x + ch.Bearing.X;
		float ypos = -y - (ch.Size.Y - ch.Bearing.Y);

		anTextureVertex v0;
		v0.Position = { xpos, -ypos - h, 0.0f };
		v0.TexCoord = mQuadTexCoords[0];
		v0.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		v0.TexIndex = (int)texIndex;
		mTextureVertices.push_back(v0);

		anTextureVertex v1;
		v1.Position = { xpos + w, -ypos - h, 0.0f };
		v1.TexCoord = mQuadTexCoords[1];
		v1.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		v1.TexIndex = (int)texIndex;
		mTextureVertices.push_back(v1);

		anTextureVertex v2;
		v2.Position = { xpos + w, -ypos, 0.0f };
		v2.TexCoord = mQuadTexCoords[2];
		v2.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		v2.TexIndex = (int)texIndex;
		mTextureVertices.push_back(v2);

		anTextureVertex v3;
		v3.Position = { xpos, -ypos, 0.0f };
		v3.TexCoord = mQuadTexCoords[3];
		v3.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		v3.TexIndex = (int)texIndex;
		mTextureVertices.push_back(v3);

		x += ch.Advance >> 6;

		mTextureIndexCount += 6;
	}
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

anUInt32 anRenderer::GetDrawCallCount() const
{
	return mDrawCallCount;
}

anUInt32 anRenderer::GetIndexCount() const
{
	return mTextureIndexCount;
}
