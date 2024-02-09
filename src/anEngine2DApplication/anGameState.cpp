#include "anGameState.h"
#include "Project/anProject.h"
#include "Device/anGPUCommands.h"
#include "Core/anInputSystem.h"
#include "Scene/anSceneManager.h"

anGameState::anGameState(anApplication* app)
	: anState(app)
{
}

anGameState::~anGameState()
{
}

void anGameState::Initialize()
{
	mCurrentPath = anFileSystem::current_path();

	mAssetsPath = mCurrentPath / "assets";

	anProjectManager::LoadProject("Application.anProj");
	mApplication->GetWindow()->SetTitle(anProjectManager::GetCurrentProject()->Name);

	const anFloat2 monitorSize = mApplication->GetWindow()->GetMonitorSize();
	const int width = (int)monitorSize.x;
	const int height = (int)monitorSize.y;

	mfWidth = (float)width;
	mfHeight = (float)height;

	anSceneManager::Load(mCurrentPath.string(), anProjectManager::GetCurrentProject()->StartScene);
	
	anSceneManager::Get()->RuntimeInitialize();
}

void anGameState::Update(float dt)
{
	anSceneManager::Get()->OnViewportSize(int(mfWidth), int(mfHeight));

	anClearColor(anSceneManager::Get()->GetClearColor());
	anEnableBlend();

	anSceneManager::Get()->RuntimeUpdate(dt);

	anRenderer2D::Get().End();
}

void anGameState::OnEvent(const anEvent& event)
{
	if (event.Type == anEvent::WindowClose)
		anSceneManager::Get()->RuntimeStop();
}

void anGameState::OnImGuiRender()
{
}
