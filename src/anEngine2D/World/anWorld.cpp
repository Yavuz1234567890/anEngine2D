#include "anWorld.h"

anWorld::anWorld()
	: mCurrentScene(nullptr)
{
}

anWorld::~anWorld()
{
}

void anWorld::SetCurrentScene(anScene* scene)
{
	mCurrentScene = scene;
}

anScene* anWorld::GetCurrentScene()
{
	return mCurrentScene;
}

void anWorld::Initialize()
{
}

void anWorld::Update(float dt)
{
}

void anWorld::Render(anRenderer& renderer)
{
	if (mCurrentScene)
		mCurrentScene->Render(renderer);
}
