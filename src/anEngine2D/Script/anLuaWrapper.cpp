#include "anLuaWrapper.h"
#include "Core/anUserInputSystem.h"
#include "Core/anLog.h"
#include "Editor/anEditorFunctions.h"
#include "Math/anMath.h"

anLuaWrapper::anLuaWrapper()
{
}

anLuaWrapper::~anLuaWrapper()
{
}

void anLuaWrapper::WrapEngine()
{
	mState.open_libraries(sol::lib::base,
		sol::lib::math,
		sol::lib::string,
		sol::lib::coroutine,
		sol::lib::package,
		sol::lib::debug,
		sol::lib::io,
		sol::lib::table,
		sol::lib::os);

	mState.set_function("logInfo", [&](const char* msg) { anUserInfo(msg); });
	mState.set_function("logError", [&](const char* msg) { anUserError(msg); });
	mState.set_function("logWarning", [&](const char* msg) { anUserWarning(msg); });

	anColor::RegisterLuaAPI(mState);
	anMath::RegisterLuaAPI(mState);
	anUserInputSystem::RegisterLuaAPI(mState);
	anEditorFunctions::RegisterLuaAPI(mState);
	anTransformComponent::RegisterLuaAPI(mState);
	anSpriteRendererComponent::RegisterLuaAPI(mState);
	anScene::RegisterLuaAPI(mState);
	anEntity::RegisterLuaAPI(mState);
}

sol::state& anLuaWrapper::GetState()
{
	return mState;
}
