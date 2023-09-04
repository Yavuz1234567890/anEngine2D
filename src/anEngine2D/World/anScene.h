#ifndef AN_SCENE_H_
#define AN_SCENE_H_

#include "anObject.h"
#include "Renderer/anRenderer.h"

class anScene
{
public:
	anScene();
	~anScene();

	void AddObject(anObject* object);
	anObject* GetObject(const anString& name);
	void DestroyObject(anObject* object);

	const anVector<anObject*>& GetObjects() const;
	void Render(anRenderer& renderer);
private:
	anVector<anObject*> mObjects;
};

#endif
