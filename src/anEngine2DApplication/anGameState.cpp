#include "anGameState.h"

#include "Project/anProject.h"
#include "Device/anGPUCommands.h"

anGameState::anGameState(anApplication* app)
	: anState(app)
{
}

anGameState::~anGameState()
{
}

void anGameState::Initialize()
{
	anProjectManager::LoadProject("Application.anProj");
	mApplication->GetWindow()->SetTitle(anProjectManager::GetCurrentProject()->Name);

	mScene = mSceneSerializer.DeserializeScene(anFileSystem::current_path(), anProjectManager::GetCurrentProject()->StartScene);
	mScene->RuntimeInitialize();

	const anFloat2 monitorSize = mApplication->GetWindow()->GetMonitorSize();
	const int width = (int)monitorSize.x;
	const int height = (int)monitorSize.y;

	mScene->OnViewportSize((anUInt32)width, (anUInt32)height);
}

void anGameState::Update(float dt)
{
	anClear();
	anEnableBlend();

	mScene->RuntimeUpdate(dt);

	anRenderer2D::Get().End();
}

void anGameState::OnEvent(const anEvent& event)
{
}

void anGameState::OnImGuiRender()
{
}
