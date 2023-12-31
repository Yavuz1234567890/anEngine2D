#ifndef AN_RENDERER_2D_H_
#define AN_RENDERER_2D_H_

#include "Device/anVertexArray.h"
#include "Device/anTexture.h"
#include "Math/anMatrix4.h"
#include "Core/anFont.h"
#include "Device/anShader.h"
#include "anCamera2D.h"

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

class anRenderer2D
{
public:
	anRenderer2D();
	~anRenderer2D();

	void Initialize();
	void Flush();
	void Start(anCamera2D& camera);
	void Start(anCamera2D& camera, const anMatrix4& transform);
	void End();
	void DrawLine(const anFloat2& start, const anFloat2& end, const anColor& color, float width = 0.5f);
	void DrawQuad(const anFloat2& pos, const anFloat2& size, const anColor& color);
	void DrawQuad(const anMatrix4& transform, const anColor& color);
	void DrawQuad(const anFloat2& pos, const anFloat2& size, float rot, const anColor& color);
	void DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anColor& color);
	void DrawTexture(anTexture* texture, const anFloat2& pos, const anFloat2& size, float rot, const anColor& color);
	void DrawTexture(anTexture* texture, const anMatrix4& transform, const anColor& color);
	void DrawTextureSub(anTexture* texture, const anFloat2& pos, const anFloat2& size, const anFloat2& spos, const anFloat2& ssize, float rot, const anColor& color);
	void DrawTextureSub(anTexture* texture, const anMatrix4& transform, const anFloat2& spos, const anFloat2& ssize, const anColor& color);
	void DrawString(const anFont& font, const anFloat2& pos, const anString& str, const anColor& color);
	void SetQuadPositions(const anFloat4& p0, const anFloat4& p1, const anFloat4& p2, const anFloat4& p3);
	anUInt32 GetDrawCallCount() const;
	anUInt32 GetIndexCount() const;
	anCamera2D& GetCamera();
	static anRenderer2D& Get();
private:
	void DrawLineVertices(anFloat2* vertices, anUInt32 size, const anColor& color);
	void StartDraw();
	anUInt32 GetTextureIndex(anTexture* texture);
private:
	anMatrix4 mMatrix;
	anCamera2D mCamera;

	anShader* mShader = nullptr;

	anFloat4 mQuadPositions[4];
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
