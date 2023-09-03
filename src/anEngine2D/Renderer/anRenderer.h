#ifndef AN_RENDERER_H_
#define AN_RENDERER_H_

#include "Device/anVertexArray.h"
#include "Device/anTexture.h"
#include "Math/anMatrix4.h"

#define anMaxQuads					20000
#define anMaxVertices				anMaxQuads * 4
#define anMaxIndices				anMaxQuads * 6
#define anMaxTextureSlots			32
#define anWhiteTextureSlot			0

struct anColorVertex
{
	anFloat3 Position;
	anFloat4 Color;
};

struct anTextureVertex
{
	anFloat3 Position;
	anFloat2 TexCoord;
	anFloat4 Color;
	int TexIndex;
};

class anRenderer
{
public:
	anRenderer();
	~anRenderer();

	void Initialize();
	void SetMatrix(const anMatrix4& matrix);
	void Flush();
	void Start();
	void End();
	void DrawLine(const anFloat2& start, const anFloat2& end, const anColor& color);
	void DrawQuad(const anFloat2& pos, const anFloat2& size, const anColor& color);
	void DrawQuad(const anFloat2& pos, const anFloat2& size, float rot, const anColor& color);
	void DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anColor& color);
	void DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, float rot, const anColor& color);
private:
	void DrawLineVertices(anFloat2* vertices, anUInt32 size, const anColor& color);
	void StartDraw();
	anUInt32 GetTextureIndex(anTexture* texture);
private:
	anMatrix4 mMatrix;

	anVertexArray* mLineVertexArray;
	anVertexBuffer* mLineVertexBuffer;
	anVector<anColorVertex> mLineVertices;
	anUInt32 mLineVertexCount = 0;

	anFloat3 mQuadPositions[4];
	anFloat2 mQuadTexCoords[4];

	anVertexArray* mTextureVertexArray;
	anIndexBuffer* mTextureIndexBuffer;
	anVertexBuffer* mTextureVertexBuffer;
	anVector<anTextureVertex> mTextureVertices;
	anUInt32 mTextureIndexCount = 0;

	anTexture* mTextures[anMaxTextureSlots];
	anUInt32 mTextureIndex = 0;
};

#endif
