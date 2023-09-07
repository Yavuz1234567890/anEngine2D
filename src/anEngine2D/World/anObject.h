#ifndef AN_OBJECT_H_
#define AN_OBJECT_H_

#include "Core/anTypes.h"

class anScene;

class anObject
{
public:
	enum : anUInt32
	{
		Line,
		Sprite,
		Text
	};

public:
	anObject(const anString& name, anUInt32 type);
	~anObject();

	void SetName(const anString& name);
	const anString& GetName() const;
	anUInt32 GetType() const;
	void SetScene(anScene* scene);
	anScene* GetScene();
	void SetVisible(bool visible);
	bool IsVisible() const;
protected:
	anString mName;
	anUInt32 mType;
	anScene* mScene;
	bool mIsVisible = true;
};

#endif
