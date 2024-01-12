#ifndef AN_SCRIPT_SYSTEM_H_
#define AN_SCRIPT_SYSTEM_H_

#include "anLuaWrapper.h"

class anScriptSystem
{
public:
	static void Initialize();
	static anLuaWrapper* GetWrapper();
private:
	static anLuaWrapper* sLuaWrapper;
};

#endif
