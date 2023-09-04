#ifndef AN_SPRITE_OBJECT_H_
#define AN_SPRITE_OBJECT_H_

#include "anObject.h"
#include "Math/anFloat4.h"
#include "Device/anTexture.h"

class anSpriteObject : public anObject
{
public:
	anSpriteObject(const anString& name);
	~anSpriteObject();

	void SetPosition(const anFloat2& pos);
	void SetSize(const anFloat2& size);
	void SetRotation(float rot);
	void SetColor(const anColor& color);
	const anFloat2& GetPosition() const;
	const anFloat2& GetSize() const;
	float GetRotation() const;
	anFloat2& GetPosition();
	anFloat2& GetSize();
	float& GetRotation();
	const anColor& GetColor() const;
	anColor& GetColor();
	void SetTexture(anTexture* texture);
	anTexture* GetTexture();
private:
	anFloat2 mPosition;
	anFloat2 mSize;
	float mRotation = 0.0f;
	anColor mColor = { 255, 255, 255, 255 };
	anTexture* mTexture = nullptr;
};

#endif
