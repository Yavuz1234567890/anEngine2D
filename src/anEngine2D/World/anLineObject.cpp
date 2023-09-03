#include "anLineObject.h"

anLineObject::anLineObject(const anString& name)
	: anObject(name, anObject::Line)
{
}

anLineObject::~anLineObject()
{
}

void anLineObject::SetStartPoint(const anFloat2& point)
{
	mStartPoint = point;
}

void anLineObject::SetEndPoint(const anFloat2& point)
{
	mEndPoint = point;
}

void anLineObject::SetColor(const anColor& color)
{
	mColor = color;
}

const anFloat2& anLineObject::GetStartPoint() const
{
	return mStartPoint;
}

const anFloat2& anLineObject::GetEndPoint() const
{
	return mEndPoint;
}

const anColor& anLineObject::GetColor() const
{
	return mColor;
}

anFloat2& anLineObject::GetStartPoint()
{
	return mStartPoint;
}

anFloat2& anLineObject::GetEndPoint()
{
	return mEndPoint;
}

anColor& anLineObject::GetColor()
{
	return mColor;
}
