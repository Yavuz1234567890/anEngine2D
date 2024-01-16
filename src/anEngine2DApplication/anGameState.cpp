#include "anGameState.h"
#include "Project/anProject.h"
#include "Device/anGPUCommands.h"
#include "Editor/anEditorFunctions.h"
#include "Core/anUserInputSystem.h"

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
		auto loadScene = [this](const anString& path) { mScene = mSceneSerializer.DeserializeScene(mCurrentPath, mAssetsPath / path); };
	
		anEditorFunctions::SetCloseApplication(closeApplication);
		anEditorFunctions::SetLoadScene(loadScene);
	}

	mCurrentPath = anFileSystem::current_path();

	mAssetsPath = mCurrentPath / "assets";

	anProjectManager::LoadProject("Application.anProj");
	mApplication->GetWindow()->SetTitle(anProjectManager::GetCurrentProject()->Name);

	const anFloat2 monitorSize = mApplication->GetWindow()->GetMonitorSize();
	const int width = (int)monitorSize.x;
	const int height = (int)monitorSize.y;

	mfWidth = (float)width;
	mfHeight = (float)height;

	mScene = mSceneSerializer.DeserializeScene(mCurrentPath, anProjectManager::GetCurrentProject()->StartScene);
	
	mScene->RuntimeInitialize();
}

void anGameState::Update(float dt)
{
	mScene->OnViewportSize(int(mfWidth), int(mfHeight));

	anClearColor(mScene->GetClearColor());
	anEnableBlend();

	mScene->RuntimeUpdate(dt);

	anRenderer2D::Get().End();
}

void anGameState::OnEvent(const anEvent& event)
{
	if (event.Type == anEvent::MouseMove)
		anUserInputSystem::SetMousePosition((event.MousePosition - anFloat2(mfWidth, mfHeight) * 0.5f) + (mScene->HasCamera() ? mScene->GetCurrentCameraPosition() : anFloat2(0.0f, 0.0f)));
}

void anGameState::OnImGuiRender()
{
}
