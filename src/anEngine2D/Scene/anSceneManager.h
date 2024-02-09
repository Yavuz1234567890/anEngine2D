#ifndef AN_SCENE_MANAGER_H_
#define AN_SCENE_MANAGER_H_

#include "anScene.h"

#include <sol/sol.hpp>

class anSceneManager
{
public:
	static void SetScene(anScene* scene);
	static void Load(const anString& location, const anString& path);
	static void Load(const anString& name);
	static void Save(const anString& location, const anString& path);
	static anScene* Get();
	static void RegisterLuaAPI(sol::state& state);
private:
	static anScene* sScene;
};

#endif
