#include "anLuaWrapper.h"
#include "Core/anInputSystem.h"
#include "Core/anLog.h"
#include "Editor/anEditorFunctions.h"

anLuaWrapper::anLuaWrapper()
{
}

anLuaWrapper::~anLuaWrapper()
{
}

void anLuaWrapper::WrapEngine()
{
	mLua.open_libraries(sol::lib::base,
		sol::lib::math,
		sol::lib::string,
		sol::lib::coroutine,
		sol::lib::package,
		sol::lib::debug,
		sol::lib::io,
		sol::lib::table,
		sol::lib::os);

	auto multOverloadsFloat2 =
		sol::overload([](const anFloat2& v1, const anFloat2& v2) -> anFloat2 { return v1 * v2; },
			[](const anFloat2& v1, float f) -> anFloat2 { return v1 * f; },
			[](float f, const anFloat2& v1) -> anFloat2 { return f * v1; });
	
	auto divOverloadsFloat2 = sol::overload([](const anFloat2& v1, const anFloat2& v2) -> anFloat2 { return v1 / v2; },
		[](const anFloat2& v1, float f) -> anFloat2 { return v1 / f; },
		[](float f, const anFloat2& v1) -> anFloat2 { return f / v1; });
	
	auto addOverloadsFloat2 =
		sol::overload([](const anFloat2& v1, const anFloat2& v2) -> anFloat2 { return v1 + v2; });
	
	auto subtractOverloadsFloat2 =
		sol::overload([](const anFloat2& v1, const anFloat2& v2) -> anFloat2 { return v1 - v2; });
	
	auto multOverloadsFloat3 =
		sol::overload([](const anFloat3& v1, const anFloat3& v2) -> anFloat3 { return v1 * v2; },
			[](const anFloat3& v1, float f) -> anFloat3 { return v1 * f; },
			[](float f, const anFloat3& v1) -> anFloat3 { return f * v1; });
	
	auto divOverloadsFloat3 = sol::overload([](const anFloat3& v1, const anFloat3& v2) -> anFloat3 { return v1 / v2; },
		[](const anFloat3& v1, float f) -> anFloat3 { return v1 / f; },
		[](float f, const anFloat3& v1) -> anFloat3 { return f / v1; });
	
	auto addOverloadsFloat3 =
		sol::overload([](const anFloat3& v1, const anFloat3& v2) -> anFloat3 { return v1 + v2; });
	
	auto subtractOverloadsFloat3 =
		sol::overload([](const anFloat3& v1, const anFloat3& v2) -> anFloat3 { return v1 - v2; });
	
	auto multOverloadsFloat4 =
		sol::overload([](const anFloat4& v1, const anFloat4& v2) -> anFloat4 { return v1 * v2; },
			[](const anFloat4& v1, float f) -> anFloat4 { return v1 * f; },
			[](float f, const anFloat4& v1) -> anFloat4 { return f * v1; });
	
	auto divOverloadsFloat4 = sol::overload([](const anFloat4& v1, const anFloat4& v2) -> anFloat4 { return v1 / v2; },
		[](const anFloat4& v1, float f) -> anFloat4 { return v1 / f; },
		[](float f, const anFloat4& v1) -> anFloat4 { return f / v1; });
	
	auto addOverloadsFloat4 =
		sol::overload([](const anFloat4& v1, const anFloat4& v2) -> anFloat4 { return v1 + v2; });
	
	auto subtractOverloadsFloat4 =
		sol::overload([](const anFloat4& v1, const anFloat4& v2) -> anFloat4 { return v1 - v2; });
	
	auto float2 = mLua.new_usertype<anFloat2>(
		"anFloat2",
		sol::constructors<anFloat2(), anFloat2(anFloat2), anFloat2(float), anFloat2(float, float)>(),
		"x", &anFloat2::x,
		"y", &anFloat2::y
	);
	
	auto float3 = mLua.new_usertype<anFloat3>(
		"anFloat3",
		sol::constructors<anFloat3(), anFloat3(anFloat3), anFloat3(float), anFloat3(float, float, float)>(),
		"x", &anFloat3::x,
		"y", &anFloat3::y,
		"z", &anFloat3::z
	);
	
	auto float4 = mLua.new_usertype<anFloat4>(
		"anFloat4",
		sol::constructors<anFloat4(), anFloat4(anFloat4), anFloat4(float), anFloat4(float, float, float, float)>(),
		"x", &anFloat4::x,
		"y", &anFloat4::y,
		"z", &anFloat4::z,
		"w", &anFloat4::w
	);

	mLua.set_function("isKey", [&](int key) { return anInputSystem::IsKey(key); });
	mLua.set_function("isKeyDown", [&](int key) { return anInputSystem::IsKeyDown(key); });
	mLua.set_function("isKeyUp", [&](int key) { return anInputSystem::IsKeyUp(key); });
	mLua.set_function("isMouseButton", [&](int button) { return anInputSystem::IsMouseButton(button); });
	mLua.set_function("isMouseButtonDown", [&](int button) { return anInputSystem::IsMouseButtonUp(button); });
	mLua.set_function("isMouseButtonUp", [&](int button) { return anInputSystem::IsMouseButtonDown(button); });
	mLua.set_function("getMousePosition", [&]() { return anInputSystem::GetMousePosition(); });
	mLua.set_function("logInfo", [&](const char* msg) { anUserInfo(msg); });
	mLua.set_function("logError", [&](const char* msg) { anUserError(msg); });
	mLua.set_function("logWarning", [&](const char* msg) { anUserWarning(msg); });

	anEditorFunctions::DefineEditorFunctions(mLua);

	auto transform = mLua.new_usertype<anTransformComponent>(
		"anTransform",
		"position", &anTransformComponent::Position,
		"size", &anTransformComponent::Size,
		"rotation", &anTransformComponent::Rotation
	);
	
	auto scene = mLua.new_usertype<anScene>(
		"anScene",
		"findEntity", &anScene::FindEntityWithTag
	);
	
	auto entity = mLua.new_usertype<anEntity>(
		"anEntity",
		"tag", &anEntity::GetTag,
		"transform", &anEntity::GetTransform,
		"scene", &anEntity::GetScene,
		"destroy", &anEntity::Destroy,
		"copy", &anEntity::Copy
	);
}

sol::state& anLuaWrapper::GetState()
{
	return mLua;
}
