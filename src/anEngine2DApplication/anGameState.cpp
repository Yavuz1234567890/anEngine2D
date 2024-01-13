#include "anGameState.h"
#include "Project/anProject.h"
#include "Device/anGPUCommands.h"
#include "Editor/anEditorFunctions.h"

anGameState::anGameState(anApplication* app)
	: anState(app)
{
}

anGameState::~anGameState()
{
}

void anGameState::Initialize()
{
	{
		auto closeApplication = [this]() { mApplication->GetWindow()->Close(); };
		auto loadScene = [this](const anString& path) { mScene = mSceneSerializer.DeserializeScene(anFileSystem::current_path(), path); };

		anEditorFunctions::SetCloseApplication(closeApplication);
		anEditorFunctions::SetLoadScene(loadScene);
	}

	anProjectManager::LoadProject("Application.anProj");
	mApplication->GetWindow()->SetTitle(anProjectManager::GetCurrentProject()->Name);

	const anFloat2 monitorSize = mApplication->GetWindow()->GetMonitorSize();
	const int width = (int)monitorSize.x;
	const int height = (int)monitorSize.y;

	mScene = mSceneSerializer.DeserializeScene(anFileSystem::current_path(), anProjectManager::GetCurrentProject()->StartScene);
	mScene->OnViewportSize((anUInt32)width, (anUInt32)height);
	
	mScene->RuntimeInitialize();
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
