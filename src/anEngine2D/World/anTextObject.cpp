#include "anTextObject.h"

anTextObject::anTextObject(const anString& name)
	: anObject(name, anObject::Text)
{
}

anTextObject::~anTextObject()
{
}

void anTextObject::SetFont(const anFont& font)
{
	mFont = font;
}

void anTextObject::SetPosition(const anFloat2& pos)
{
	mPosition = pos;
}

void anTextObject::SetColor(const anColor& color)
{
	mColor = color;
}

void anTextObject::SetText(const anString& text)
{
	mText = text;
}

anFont& anTextObject::GetFont()
{
	return mFont;
}

anFloat2& anTextObject::GetPosition()
{
	return mPosition;
}

anColor& anTextObject::GetColor()
{
	return mColor;
}

anString& anTextObject::GetText()
{
	return mText;
}

const anFont& anTextObject::GetFont() const
{
	return mFont;
}

const anFloat2& anTextObject::GetPosition() const
{
	return mPosition;
}

const anColor& anTextObject::GetColor() const
{
	return mColor;
}

const anString& anTextObject::GetText() const
{
	return mText;
}
