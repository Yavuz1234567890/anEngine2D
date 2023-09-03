#include "anScene.h"
#include "anSpriteObject.h"
#include "anLineObject.h"

anScene::anScene()
{
}

anScene::~anScene()
{
}

void anScene::AddObject(anObject* object)
{
	object->SetScene(this);
	mObjects.push_back(object);
}

anObject* anScene::GetObject(const anString& name)
{
	for (anObject* object : mObjects)
	{
		if (object->GetName() == name)
			return object;
	}

	return nullptr;
}

const anVector<anObject*>& anScene::GetObjects() const
{
	return mObjects;
}

void anScene::Render(anRenderer& renderer)
{
	for (anObject* object : mObjects)
	{
		switch (object->GetType())
		{
		case anObject::Sprite:
		{
			anSpriteObject* sprite = (anSpriteObject*)object;
			if (!sprite->GetTexture())
				break;

			renderer.DrawTexture(sprite->GetTexture(), sprite->GetPosition(), sprite->GetSize(), sprite->GetRotation(), sprite->GetColor());
		}
		break;
		case anObject::Line:
		{
			anLineObject* line = (anLineObject*)object;

			renderer.DrawLine(line->GetStartPoint(), line->GetEndPoint(), line->GetColor());
		}
		break;
		}
	}
}
