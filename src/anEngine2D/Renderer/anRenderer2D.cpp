#include "anRenderer2D.h"
#include "Device/anShaders.h"
#include "Core/anMessage.h"
#include "Device/anGPUCommands.h"

static anRenderer2D sRenderer2D;

anRenderer2D& anRenderer2D::Get()
{
	return sRenderer2D;
}

anRenderer2D::anRenderer2D()
{
	for (anUInt32 i = 0; i < anMaxTextureSlots; i++)
		mTextures[i] = nullptr;
}

anRenderer2D::~anRenderer2D()
{
}

void anRenderer2D::Initialize()
{
	mShader = anGetBatchShader();

	mQuadPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	mQuadPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
	mQuadPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
	mQuadPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

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

void anRenderer2D::Flush()
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

void anRenderer2D::Start(anCamera2D& camera)
{
	mCamera = camera;
	mMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
}

void anRenderer2D::Start(anCamera2D& camera, const anMatrix4& transform)
{
	mCamera = camera;
	mMatrix = camera.GetProjectionMatrix() * glm::inverse(transform);
}

void anRenderer2D::End()
{
	Flush();
}

void anRenderer2D::StartDraw()
{
	if (mTextureIndexCount >= anMaxIndices || mTextureIndex >= anMaxTextureSlots)
		Flush();
}

void anRenderer2D::DrawLine(const anFloat2& start, const anFloat2& end, const anColor& color, float width, bool addToStats)
{
	StartDraw();
	anFloat2 normal = glm::normalize(anFloat2(end.y - start.y, -(end.x - start.x))) * width;
	
	anTextureVertex v0;
	v0.Position = { start.x + normal.x, start.y + normal.y, 0.0f };
	v0.TexCoord = mQuadTexCoords[0];
	v0.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	v0.TexIndex = anWhiteTextureSlot;
	mTextureVertices.push_back(v0);

	anTextureVertex v1;
	v1.Position = { end.x + normal.x, end.y + normal.y, 0.0f };
	v1.TexCoord = mQuadTexCoords[1];
	v1.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	v1.TexIndex = anWhiteTextureSlot;
	mTextureVertices.push_back(v1);

	anTextureVertex v2;
	v2.Position = { end.x - normal.x, end.y - normal.y, 0.0f };
	v2.TexCoord = mQuadTexCoords[2];
	v2.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	v2.TexIndex = anWhiteTextureSlot;
	mTextureVertices.push_back(v2);

	anTextureVertex v3;
	v3.Position = { start.x - normal.x, start.y - normal.y, 0.0f };
	v3.TexCoord = mQuadTexCoords[3];
	v3.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
	v3.TexIndex = anWhiteTextureSlot;
	mTextureVertices.push_back(v3);

	mTextureIndexCount += 6;

	if (addToStats)
	{
		mVertexCount += 4;
		mIndexCount += 6;
	}
}

void anRenderer2D::DrawQuad(const anFloat2& pos, const anFloat2& size, const anColor& color, bool addToStats)
{
	anFloat2 vertices[4];
	for (anUInt32 i = 0; i < 4; i++)
		vertices[i] = { pos.x + mQuadPositions[i].x * size.x, pos.y + mQuadPositions[i].y * size.y };
	
	DrawLineVertices(vertices, 4, color, addToStats);
}

void anRenderer2D::DrawQuad(const anMatrix4& transform, const anColor& color, bool addToStats)
{
	anFloat2 vertices[4];
	for (anUInt32 i = 0; i < 4; i++)
	{
		anFloat3 vert = transform * mQuadPositions[i];

		vertices[i].x = vert.x;
		vertices[i].y = vert.y;
	}

	DrawLineVertices(vertices, 4, color, addToStats);
}

void anRenderer2D::DrawQuad(const anFloat2& pos, const anFloat2& size, float rot, const anColor& color, bool addToStats)
{
	anMatrix4 transform = glm::translate(anMatrix4(1.0f), { pos.x, pos.y, 0.0f })
		* glm::rotate(anMatrix4(1.0f), glm::radians(rot), { 0.0f, 0.0f, 1.0f })
		* glm::scale(anMatrix4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, color, addToStats);
}

void anRenderer2D::DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anColor& color, bool addToStats)
{
	StartDraw();

	anUInt32 index = GetTextureIndex(texture);

	for (anUInt32 i = 0; i < 4; i++)
	{
		anTextureVertex vertex;
		vertex.Position = { pos.x + mQuadPositions[i].x * size.x, pos.y + mQuadPositions[i].y * size.y, 0.0f };
		vertex.TexCoord = mQuadTexCoords[i];
		vertex.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		vertex.TexIndex = (int)index;
		mTextureVertices.push_back(vertex);
	}

	mTextureIndexCount += 6;

	if (addToStats)
	{
		mVertexCount += 4;
		mIndexCount += 6;
	}
}

void anRenderer2D::DrawTexture(anTexture* texture, const anMatrix4& transform, const anColor& color, bool addToStats)
{
	StartDraw();

	anUInt32 index = GetTextureIndex(texture);
	for (anUInt32 i = 0; i < 4; i++)
	{
		anTextureVertex vertex;
		vertex.Position = transform * mQuadPositions[i];
		vertex.TexCoord = mQuadTexCoords[i];
		vertex.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		vertex.TexIndex = (int)index;
		mTextureVertices.push_back(vertex);
	}

	mTextureIndexCount += 6;

	if (addToStats)
	{
		mVertexCount += 4;
		mIndexCount += 6;
	}
}

void anRenderer2D::DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, float rot, const anColor& color, bool addToStats)
{
	anMatrix4 transform = glm::translate(anMatrix4(1.0f), { pos.x, pos.y, 0.0f })
		* glm::rotate(anMatrix4(1.0f), glm::radians(rot), { 0.0f, 0.0f, 1.0f })
		* glm::scale(anMatrix4(1.0f), { size.x, size.y, 1.0f });

	DrawTexture(texture, transform, color, addToStats);
}

void anRenderer2D::DrawTextureSub(anTexture* texture, const anMatrix4& transform, const anFloat2& spos, const anFloat2& ssize, const anColor& color, bool addToStats)
{
	StartDraw();

	const int texWidth = (int)texture->GetWidth();
	const int texHeight = (int)texture->GetHeight();

	anUInt32 index = GetTextureIndex(texture);
	for (anUInt32 i = 0; i < 4; i++)
	{
		anTextureVertex vertex;
		vertex.Position = transform * mQuadPositions[i];
		vertex.TexCoord = { (mQuadTexCoords[i].x * ssize.x + spos.x) / texWidth, (mQuadTexCoords[i].y * ssize.y + spos.y) / texHeight };
		vertex.Color = { (float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f, (float)color.A / 255.0f };
		vertex.TexIndex = (int)index;
		mTextureVertices.push_back(vertex);
	}

	mTextureIndexCount += 6;

	if (addToStats)
	{
		mVertexCount += 4;
		mIndexCount += 6;
	}
}

void anRenderer2D::DrawTextureSub(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anFloat2& spos, const anFloat2& ssize, float rot, const anColor& color, bool addToStats)
{
	anMatrix4 transform = glm::translate(anMatrix4(1.0f), { pos.x, pos.y, 0.0f })
		* glm::rotate(anMatrix4(1.0f), glm::radians(rot), { 0.0f, 0.0f, 1.0f })
		* glm::scale(anMatrix4(1.0f), { size.x, size.y, 1.0f });

	DrawTextureSub(texture, transform, spos, ssize, color, addToStats);
}

void anRenderer2D::DrawString(const anFont& font, const anFloat2& pos, const anString& str, const anColor& color, bool addToStats)
{
	anUInt32 spaceAdvance = font.GetCharacter(' ').Advance;
	anUInt32 tabAdvance = spaceAdvance * 4;
    
	float x = pos.x;
	float y = pos.y;
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
			x = pos.x;
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

		mVertexCount += 4;
		mIndexCount += 6;
	}
}

void anRenderer2D::SetQuadPositions(const anFloat4& p0, const anFloat4& p1, const anFloat4& p2, const anFloat4& p3)
{
	mQuadPositions[0] = p0;
	mQuadPositions[1] = p1;
	mQuadPositions[2] = p2;
	mQuadPositions[3] = p3;
}

void anRenderer2D::DrawLineVertices(anFloat2* vertices, anUInt32 size, const anColor& color, bool addToStats)
{
	for (anUInt32 i = 0; i < size - 1; i++)
		DrawLine(vertices[i], vertices[i + 1], color, 0.5f, addToStats);

	DrawLine(vertices[size - 1], vertices[0], color, 0.5f, addToStats);
}

anUInt32 anRenderer2D::GetTextureIndex(anTexture* texture)
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

anUInt32 anRenderer2D::GetDrawCallCount() const
{
	return mDrawCallCount;
}

anUInt32 anRenderer2D::GetIndexCount() const
{
	return mIndexCount;
}

anUInt32 anRenderer2D::GetVertexCount() const
{
	return mVertexCount;
}

anCamera2D& anRenderer2D::GetCamera()
{
	return mCamera;
}

void anRenderer2D::ResetStats()
{
	mDrawCallCount = 0;
	mIndexCount = 0;
	mVertexCount = 0;
}
