#ifndef AN_PROJECT_H_
#define AN_PROJECT_H_

#include "Core/anTypes.h"
#include "Core/anFileSystem.h"

struct anProject
{
	anString Name;
	anString StartScene;
	anFileSystem::path Location;
	anFileSystem::path FullPath;
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
