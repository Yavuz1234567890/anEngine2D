#include "anScriptSystem.h"

anLuaWrapper* anScriptSystem::sLuaWrapper = nullptr;
static bool sScriptSystemInitialized = false;

void anScriptSystem::Initialize()
{
	if (!sScriptSystemInitialized)
	{
		sLuaWrapper = new anLuaWrapper();
		sLuaWrapper->WrapEngine();
		sScriptSystemInitialized = true;
	}
}

anLuaWrapper* anScriptSystem::GetWrapper()
{
	return sLuaWrapper;
}
