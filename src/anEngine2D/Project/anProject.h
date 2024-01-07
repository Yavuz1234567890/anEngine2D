#ifndef AN_PROJECT_H_
#define AN_PROJECT_H_

#include "Core/anTypes.h"

struct anProject
{
	anString Name;
	anString StartScene;
	anString Location;
	anString FullPath;
};

class anProjectManager
{
public:
	static void SetCurrentProject(anProject* project);
	static anProject* GetCurrentProject();
	static void LoadProject(const anString& path);
	static void SaveProject(const anString& path);
	static bool IsProjectActive();
private:
	static anProject* sCurrentProject;
};

#endif
