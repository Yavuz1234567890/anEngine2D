#include "anApplication.h"
#include "Device/anShaders.h"
#include "Device/anTexture.h"
#include "anFont.h"
#include "State/anStateManager.h"
#include "anInputSystem.h"
#include "anControllerDevice.h"
#include "anMessage.h"
#include "Project/anProject.h"

static anApplication* sInstance = nullptr;

anApplication::anApplication(const anApplicationCreationDescription& desc)
	: mFramesPerSecond(0)
{
	sInstance = this;

	mApplicationDesc = desc;
	mLogFile.open(desc.Title + ".log");
}

anApplication::~anApplication()
{
}

void anApplication::Start()
{
	mStateManager = new anStateManager(this);

	auto onEvent = [this](const anEvent& event) { AOnEvent(event); };

	{
		auto editorInfo = [this](const anString& msg) { EditorInfo(msg); };
		auto editorError = [this](const anString& msg) { EditorError(msg); };
		auto editorWarning = [this](const anString& msg) { EditorWarning(msg); };

		auto userInfo = [this](const anString& msg) { UserInfo(msg); };
		auto userError = [this](const anString& msg) { UserError(msg); };
		auto userWarning = [this](const anString& msg) { UserWarning(msg); };

		anSetDefaultEditorLogCallback({ editorInfo, editorError, editorWarning });
		anSetDefaultUserLogCallback({ userInfo, userError, userWarning });
	}

	anDisplayResolution::Initialize();
	mWindow = anCreateWindow(mApplicationDesc.Title, mApplicationDesc.Width, mApplicationDesc.Height, onEvent, mApplicationDesc.WindowResizable, mApplicationDesc.WindowMaximized);
	anTexture::Initialize();
	anFont::Initialize();
	anSound::InitializeFMOD();
	anInitializeShaders();
	mStateManager->Initialize();
	mImGui.Initialize(mWindow);
	anRenderer2D::Get().Initialize();
	anControllerDevice::Initialize();
	anInputSystem::Initialize();
	anScriptSystem::Initialize();
	Initialize();

	float secondTimer = 0.0f;
	while (mWindow->IsRunning())
	{
		const float dt = mTimer.Tick();
		secondTimer += dt;
		if (secondTimer >= 1.0f)
		{
			mFramesPerSecond = int(1.0f / dt);
			secondTimer = 0.0f;
		}

		if (!mMinimized)
		{
			anControllerDevice::Update(dt);
			mStateManager->Update(dt);
			Update(dt);
			anInputSystem::Update(dt);

			mImGui.Start();
			OnImGui();
			mImGui.End();
		}

		mWindow->Present();
	}

	anSound::ShutdownFMOD();
}

void anApplication::AOnEvent(const anEvent& event)
{
	if (event.Type == anEvent::WindowSize)
		mMinimized = event.WindowWidth == 0 || event.WindowHeight == 0;

	mStateManager->OnEvent(event);
	anInputSystem::OnEvent(event);
	OnEvent(event);
}

void anApplication::OnImGui()
{
	OnImGuiRender();
	mStateManager->OnImGuiRender();
}

void anApplication::SetCurrentState(anState* state)
{
	mStateManager->SetCurrentState(state);
}

anState* anApplication::GetCurrentState()
{
	return mStateManager->GetCurrentState();
}

anWindow* anApplication::GetWindow()
{
	return mWindow;
}

void anApplication::LogWrite(const anString& msg)
{
	if (!mLogFile.is_open())
		return;

	mLogFile << msg << "\n";
}

int anApplication::GetFramesPerSecond() const
{
	return mFramesPerSecond;
}

void anApplication::EditorInfo(const anString& msg)
{
	const anString s = "[Info] From: Editor, " + msg;
	anMessage(s);
	LogWrite(s);
}

void anApplication::EditorError(const anString& msg)
{
	const anString s = "[Error] From: Editor, " + msg;
	anMessage(s);
	LogWrite(s);
}

void anApplication::EditorWarning(const anString& msg)
{
	const anString s = "[Warning] From: Editor, " + msg;
	anMessage(s);
	LogWrite(s);
}

void anApplication::UserInfo(const anString& msg)
{
	const anString s = "[Info] From: User, " + msg;
	anMessage(s);
	LogWrite(s);
}

void anApplication::UserError(const anString& msg)
{
	const anString s = "[Error] From: User, " + msg;
	anMessage(s);
	LogWrite(s);
}

void anApplication::UserWarning(const anString& msg)
{
	const anString s = "[Warning] From: User, " + msg;
	anMessage(s);
	LogWrite(s);
}

anApplication* anApplication::Get()
{
	return sInstance;
}

void anApplication::RegisterLuaAPI(sol::state& state)
{
	state.set_function("closeApplication", [&]() { Get()->GetWindow()->Close(); });
	state.set_function("setVSync", [&](bool vsync) { Get()->GetWindow()->SetVSync(vsync); });
	state.set_function("getVSync", [&]() { return Get()->GetWindow()->IsVSync(); });
}
