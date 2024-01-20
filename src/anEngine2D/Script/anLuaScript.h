#ifndef AN_LUA_SCRIPT_H_
#define AN_LUA_SCRIPT_H_

#include "Core/anFileSystem.h"
#include "Scene/anEntity.h"

#include <sol/sol.hpp>

class anLuaScript
{
public:
	anLuaScript();
	~anLuaScript();

	void LoadScript(const anFileSystem::path& scriptLocation, const anFileSystem::path& editorLocation);
	void Initialize(anEntity entity);
	void Update(float dt);
	const anFileSystem::path& GetPath() const;
	const anFileSystem::path& GetEditorPath() const;
	void SetEditorPath(const anFileSystem::path& path);
	void Reload();
	sol::table& GetScriptClass();
private:
	anFileSystem::path mScriptPath;
	anFileSystem::path mEditorPath;
	sol::table mSelf;
	sol::table mScriptClass;

	sol::protected_function mSetupFunction;
	sol::protected_function mInitializeFunction;
	sol::protected_function mUpdateFunction;

	anString mFileSource;
};

#endif
