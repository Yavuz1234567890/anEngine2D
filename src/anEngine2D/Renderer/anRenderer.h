#ifndef AN_RENDERER_H_
#define AN_RENDERER_H_

#include "Device/anVertexArray.h"
#include "Device/anTexture.h"
#include "Math/anMatrix4.h"
#include "Core/anFont.h"
#include "Device/anShader.h"

#define anMaxQuads					20000
#define anMaxVertices				anMaxQuads * 4
#define anMaxIndices				anMaxQuads * 6
#define anMaxTextureSlots			32
#define anWhiteTextureSlot			0

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
	void DrawLine(const anFloat2& start, const anFloat2& end, const anColor& color, float width = 1.0f);
	void DrawQuad(const anFloat2& pos, const anFloat2& size, const anColor& color);
	void DrawQuad(const anFloat2& pos, const anFloat2& size, float rot, const anColor& color);
	void DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anColor& color);
	void DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, float rot, const anColor& color);
	void DrawTextureSub(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anFloat2& spos, const anFloat2& ssize, float rot, const anColor& color);
	void DrawString(const anFont& font, const anFloat2& pos, const anString& str, const anColor& color);
	anUInt32 GetDrawCallCount() const;
	anUInt32 GetIndexCount() const;
private:
	void DrawLineVertices(anFloat2* vertices, anUInt32 size, const anColor& color);
	void StartDraw();
	anUInt32 GetTextureIndex(anTexture* texture);
private:
	anMatrix4 mMatrix;

	anShader* mShader = nullptr;

	anFloat3 mQuadPositions[4];
	anFloat2 mQuadTexCoords[4];

	anVertexArray* mTextureVertexArray = nullptr;
	anIndexBuffer* mTextureIndexBuffer = nullptr;
	anVertexBuffer* mTextureVertexBuffer = nullptr;
	anVector<anTextureVertex> mTextureVertices;
	anUInt32 mTextureIndexCount = 0;

	anTexture* mTextures[anMaxTextureSlots];
	anUInt32 mTextureIndex = 0;

	anUInt32 mDrawCallCount = 0;
};

#endif
