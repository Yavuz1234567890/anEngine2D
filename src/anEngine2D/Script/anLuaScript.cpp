#include "anLuaScript.h"
#include "anScriptSystem.h"
#include "Core/anMessage.h"

anLuaScript::anLuaScript()
{
}

anLuaScript::~anLuaScript()
{
}

void anLuaScript::LoadScript(const anFileSystem::path& scriptLocation, const anFileSystem::path& editorLocation)
{
	anInputFile file{ scriptLocation };
	if (!file.good())
		return;
	
	anStringStream srcCodeStream;
	srcCodeStream << file.rdbuf();
	
	anString srcCode = srcCodeStream.str();
	
	auto& state = anScriptSystem::GetWrapper()->GetState();
	
	try 
	{ state.script(srcCode); }
	catch (const std::exception& e)
	{ anShowMessageBox("Error", "Lua script error: " + anString(e.what()), anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error); }
	
	anString scriptName = scriptLocation.stem().string();
	for (anUInt64 i = 0; i < scriptName.size(); i++)
	{
		if (scriptName[i] == ' ')
			scriptName.erase(scriptName.begin() + i);
	}

	mSetupFunction = state[scriptName]["setup"];
	mInitializeFunction = state[scriptName]["initialize"];
	mUpdateFunction = state[scriptName]["update"];
	if (!mInitializeFunction.valid() || !mUpdateFunction.valid() || !mSetupFunction.valid())
		return;

	mScriptPath = scriptLocation;
	mEditorPath = editorLocation;
}

void anLuaScript::Initialize(anEntity entity)
{
	auto setup = mSetupFunction();
	if (setup.valid())
	{
		mSelf = setup;
		mSelf["owner"] = entity;
	}
	
	sol::load_result lr = anScriptSystem::GetWrapper()->GetState().load("local self = ...;\n");
	lr(mSelf);
	
	auto initialize = mInitializeFunction(mSelf);
	if (!initialize.valid())
		return;
}

void anLuaScript::Update(float dt)
{
	auto update = mUpdateFunction(mSelf, dt);
	if (!update.valid())
		return;
}

const anFileSystem::path& anLuaScript::GetPath() const
{
	return mScriptPath;
}

const anFileSystem::path& anLuaScript::GetEditorPath() const
{
	return mEditorPath;
}
