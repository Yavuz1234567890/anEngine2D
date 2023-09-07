#include "anObject.h"
#include "anScene.h"

anObject::anObject(const anString& name, anUInt32 type)
	: mName(name)
	, mType(type)
	, mScene(nullptr)
{
}

anObject::~anObject()
{
}

void anObject::SetName(const anString& name)
{
	mName = name;
}

const anString& anObject::GetName() const
{
	return mName;
}

anUInt32 anObject::GetType() const
{
	return mType;
}

void anObject::SetScene(anScene* scene)
{
	mScene = scene;
}

anScene* anObject::GetScene()
{
	return mScene;
}

void anObject::SetVisible(bool visible)
{
	mIsVisible = visible;
}

bool anObject::IsVisible() const
{
	return mIsVisible;
}
