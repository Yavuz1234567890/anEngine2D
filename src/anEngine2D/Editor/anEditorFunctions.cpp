#include "anEditorFunctions.h"

namespace anEditorFunctions
{
	static struct
	{
		anFunction<void()> CloseApplicationFunction;
		anFunction<void(const anString&)> LoadSceneFunction;
		anFunction<void(bool)> SetVSyncFunction;
		anFunction<bool()> GetVSyncFunction;
	} sFunctions;

	void SetCloseApplication(const anFunction<void()>& fn)
	{
		sFunctions.CloseApplicationFunction = fn;
	}

	void SetLoadScene(const anFunction<void(const anString&)>& fn)
	{
		sFunctions.LoadSceneFunction = fn;
	}

	void SetSetVSync(const anFunction<void(bool)>& fn)
	{
		sFunctions.SetVSyncFunction = fn;
	}

	void SetGetVSync(const anFunction<bool()>& fn)
	{
		sFunctions.GetVSyncFunction = fn;
	}

	void CloseApplication()
	{
		if (sFunctions.CloseApplicationFunction)
			sFunctions.CloseApplicationFunction();
	}

	void LoadScene(const anString& scene)
	{
		if (sFunctions.LoadSceneFunction)
			sFunctions.LoadSceneFunction(scene);
	}

	void SetVSync(bool vsync)
	{
		if (sFunctions.SetVSyncFunction)
			sFunctions.SetVSyncFunction(vsync);
	}

	bool GetVSync()
	{
		if (sFunctions.GetVSyncFunction)
			return sFunctions.GetVSyncFunction();

		return false;
	}

	void RegisterLuaAPI(sol::state& state)
	{
		state.set_function("closeApplication", [&]() { anEditorFunctions::CloseApplication(); });
		state.set_function("loadScene", [&](const char* msg) { anEditorFunctions::LoadScene(msg); });
		state.set_function("setVSync", [&](bool vsync) { anEditorFunctions::SetVSync(vsync); });
		state.set_function("getVSync", [&]() { return anEditorFunctions::GetVSync(); });
	}
}
