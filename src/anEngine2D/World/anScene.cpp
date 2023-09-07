#include "anScene.h"
#include "anSpriteObject.h"
#include "anLineObject.h"
#include "anTextObject.h"

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

void anScene::DestroyObject(anObject* object)
{
	for (anUInt32 i = 0; i < mObjects.size(); i++)
	{
		if (mObjects[i] == object)
		{
			mObjects.erase(mObjects.begin() + i);
			break;
		}
	}
}

const anVector<anObject*>& anScene::GetObjects() const
{
	return mObjects;
}

void anScene::Render(anRenderer& renderer)
{
	for (anObject* object : mObjects)
	{
		if (!object->IsVisible())
			continue;

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
		case anObject::Text:
		{
			anTextObject* text = (anTextObject*)object;
			if (!text->GetFont().IsLoaded())
				break;

			renderer.DrawString(text->GetFont(), text->GetPosition(), text->GetText(), text->GetColor());
		}
		break;
		}
	}
}
