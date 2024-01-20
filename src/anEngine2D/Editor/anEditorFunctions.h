#ifndef AN_EDITOR_FUNCTIONS_H_
#define AN_EDITOR_FUNCTIONS_H_

#include "Core/anTypes.h"

#include <sol/sol.hpp>

namespace anEditorFunctions
{
	void SetCloseApplication(const anFunction<void()>& fn);
	void SetLoadScene(const anFunction<void(const anString&)>& fn);
	void SetSetVSync(const anFunction<void(bool)>& fn);
	void SetGetVSync(const anFunction<bool()>& fn);
	void CloseApplication();
	void LoadScene(const anString& scene);
	void SetVSync(bool vsync);
	bool GetVSync();
	void RegisterLuaAPI(sol::state& state);
}

#endif
