#include "anEditorFunctions.h"

#ifdef AN_EDITOR

namespace anEditorFunctions
{
	static struct
	{
		anFunction<void()> CloseApplicationFunction;
		anFunction<void(const anString&)> LoadSceneFunction;
	} sFunctions;

	void SetCloseApplication(const anFunction<void()>& fn)
	{
		sFunctions.CloseApplicationFunction = fn;
	}

	void SetLoadScene(const anFunction<void(const anString&)>& fn)
	{
		sFunctions.LoadSceneFunction = fn;
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
}

#endif
