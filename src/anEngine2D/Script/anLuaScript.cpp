#include "anLuaScript.h"
#include "anScriptSystem.h"
#include "Core/anLog.h"

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
	{
		anEditorError("Couldn't find script " + scriptLocation.string());
		return;
	}
	
	anStringStream srcCodeStream;
	srcCodeStream << file.rdbuf();
	
	mFileSource = srcCodeStream.str();
	
	auto& state = anScriptSystem::GetWrapper()->GetState();
	
	try 
	{ 
		state.script(mFileSource); 

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
	catch (const std::exception& e)
	{ anEditorError("Error from " + scriptLocation.filename().string() + " script: " + anString(e.what())); }
}

void anLuaScript::Initialize(anEntity entity)
{
	if (mSetupFunction)
	{
		auto setup = mSetupFunction();
		if (setup.valid())
		{
			mSelf = setup;
			mSelf["owner"] = entity;
		}

		sol::load_result lr = anScriptSystem::GetWrapper()->GetState().load("local self = ...;\n");
		lr(mSelf);
	}
	
	if (mInitializeFunction)
		auto initialize = mInitializeFunction(mSelf);
}

void anLuaScript::Update(float dt)
{
	if (mUpdateFunction)
		auto update = mUpdateFunction(mSelf, dt);
}

const anFileSystem::path& anLuaScript::GetPath() const
{
	return mScriptPath;
}

const anFileSystem::path& anLuaScript::GetEditorPath() const
{
	return mEditorPath;
}

void anLuaScript::SetEditorPath(const anFileSystem::path& path)
{
	mEditorPath = path;
}

void anLuaScript::Reload()
{
	LoadScript(mScriptPath, mEditorPath);
}
