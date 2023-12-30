#include "anStateManager.h"

anStateManager::anStateManager(anApplication* app)
	: mApplication(app)
{
}

anStateManager::~anStateManager()
{
}

void anStateManager::SetCurrentState(anState* state)
{
	if (state == nullptr)
		return;

	mCurrentState = state;
	mCurrentState->Initialize();
}

anState* anStateManager::GetCurrentState()
{
	return mCurrentState;
}

anApplication* anStateManager::GetApplication()
{
	return mApplication;
}

void anStateManager::Initialize()
{
}

void anStateManager::Update(float dt)
{
	if (mCurrentState)
		mCurrentState->Update(dt);
}

void anStateManager::OnEvent(const anEvent& event)
{
	if (mCurrentState)
		mCurrentState->OnEvent(event);
}

void anStateManager::Render2D(anRenderer2D& renderer)
{
	if (mCurrentState)
		mCurrentState->Render2D(renderer);
}

void anStateManager::OnImGuiRender()
{
	if (mCurrentState)
		mCurrentState->OnImGuiRender();
}
