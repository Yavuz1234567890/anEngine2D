#include "anSceneManager.h"
#include "anSceneSerializer.h"
#include "Core/anLog.h"

anScene* anSceneManager::sScene = nullptr;

void anSceneManager::SetScene(anScene* scene)
{
	sScene = scene;
}

void anSceneManager::Load(const anString& location, const anString& path)
{
	SetScene(anGlobalSceneSerializer.DeserializeScene(location, path));
}

void anSceneManager::Load(const anString& name)
{
	const anFileSystem::path loc = anFileSystem::current_path();
	SetScene(anGlobalSceneSerializer.DeserializeScene(loc, loc / "assets" / name));
	sScene->RuntimeInitialize();
}

void anSceneManager::Save(const anString& location, const anString& path)
{
	if (!Get())
		return;

	anGlobalSceneSerializer.SerializeScene(location, Get(), path);
}

anScene* anSceneManager::Get()
{
	return sScene;
}

void anSceneManager::RegisterLuaAPI(sol::state& state)
{
	state.set_function("loadScene", [&](const char* fileName) { Load(fileName); });
}
