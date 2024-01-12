#ifndef AN_LUA_WRAPPER_H_
#define AN_LUA_WRAPPER_H_

#include "Scene/anEntity.h"
#include "Math/anMatrix4.h"

#include <sol/sol.hpp>

class anLuaWrapper
{
public:
	anLuaWrapper();
	~anLuaWrapper();

	void WrapEngine();
	sol::state& GetState();
private:
	sol::state mLua;
};

#endif
