#ifndef AN_WORLD_H_
#define AN_WORLD_H_

#include "anScene.h"

class anWorld
{
public:
	anWorld();
	~anWorld();

	void SetCurrentScene(anScene* scene);
	anScene* GetCurrentScene();
	void Initialize();
	void Update(float dt);
	void Render(anRenderer& renderer);
private:
	anScene* mCurrentScene;
};

#endif
