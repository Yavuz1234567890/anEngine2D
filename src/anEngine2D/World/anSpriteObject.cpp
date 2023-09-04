#include "anSpriteObject.h"

anSpriteObject::anSpriteObject(const anString& name)
	: anObject(name, anObject::Sprite)
{
}

anSpriteObject::~anSpriteObject()
{
}

void anSpriteObject::SetPosition(const anFloat2& pos)
{
	mPosition = pos;
}

void anSpriteObject::SetSize(const anFloat2& size)
{
	mSize = size;
}

void anSpriteObject::SetRotation(float rot)
{
	mRotation = rot;
}

void anSpriteObject::SetColor(const anColor& color)
{
	mColor = color;
}

const anFloat2& anSpriteObject::GetPosition() const
{
	return mPosition;
}

const anFloat2& anSpriteObject::GetSize() const
{
	return mSize;
}

float anSpriteObject::GetRotation() const
{
	return mRotation;
}

anFloat2& anSpriteObject::GetPosition()
{
	return mPosition;
}

anFloat2& anSpriteObject::GetSize()
{
	return mSize;
}

float& anSpriteObject::GetRotation()
{
	return mRotation;
}

const anColor& anSpriteObject::GetColor() const
{
	return mColor;
}

anColor& anSpriteObject::GetColor()
{
	return mColor;
}

void anSpriteObject::SetTexture(anTexture* texture)
{
	mTexture = texture;
}

anTexture* anSpriteObject::GetTexture()
{
	return mTexture;
}
