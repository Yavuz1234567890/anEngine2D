#ifndef AN_EDITOR_FUNCTIONS_H_
#define AN_EDITOR_FUNCTIONS_H_

#ifdef EDITOR

#include "Core/anTypes.h"

namespace anEditorFunctions
{
	void SetCloseApplication(const anFunction<void()>& fn);
	void SetLoadScene(const anFunction<void(const anString&)>& fn);

	void CloseApplication();
	void LoadScene(const anString& scene);
}

#endif

#endif
