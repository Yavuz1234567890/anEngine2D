#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Renderer/anRenderer.h"
#include "Device/anGPUCommands.h"
#include "World/anWorld.h"
#include "World/anObject.h"
#include "World/anSpriteObject.h"
#include "World/anLineObject.h"
#include "World/anTextObject.h"
#include "Core/anMessage.h"
#include "Core/anKeyCodes.h"
#include "State/anStateManager.h"
#include "Math/anMath.h"
#include "TestState.h"

class Application : public anApplication
{
public:
	Application()
		: anApplication({ "anEngine2D Application", 1200, 700, false })
	{
	}

	~Application()
	{
	}

	void Initialize() override
	{		
		anInitializeRandomDevice();

		mWindow->SetWindowIcon("assets/icon.jpg");

		mTest = anLoadTexture("assets/test.png");

		mRaleway.Load("assets/Raleway-Regular.ttf", 18);
		mTestSound.Load("assets/jaguar.wav");

		mWorld = new anWorld();

		mRenderer.Initialize();

		mfWidth = (float)mApplicationDesc.Width;
		mfHeight = (float)mApplicationDesc.Height;

		mProjection = anMatrix4::Ortho(mfWidth * -0.5f, mfWidth * 0.5f, mfHeight * 0.5f, mfHeight * -0.5f, -1.0f, 1.0f);
		mRenderer.SetMatrix(mProjection);

		mWorld->Initialize();

		SetCurrentState<TestState>();
	}

	void Update(float dt) override
	{
		mWorld->Update(dt);
		
		anClearColor({ 255, 0, 0 });
		anEnableBlend();

		mRenderer.Start();

		anApplication::Render(mRenderer);
		mWorld->Render(mRenderer);

		mRenderer.DrawString(mRaleway, { 100.0f, 100.0f }, "FPS: " + anToString(mFramesPerSecond), { 255, 0, 255, 255 });
		mRenderer.DrawTexture(anTexture::GetWhiteTexture(), { 0, 0 }, { 100, 100 }, { 255, 255, 255 });
		mRenderer.DrawTexture(mTest, { 0, 0 }, { 642.0f, 313.0f }, 0.0f, { 255, 255, 255 });

		mRenderer.End();
	}
	
	void OnEvent(const anEvent& event) override
	{
		if (event.Type == anEvent::KeyDown)
		{
			if (event.KeyCode == anKeySpace)
			{
				mTestSound.Play();
			}
		}
	}

private:
	anRenderer mRenderer;

	anMatrix4 mProjection;

	anFont mRaleway;

	anScene* mScene = nullptr;
	anWorld* mWorld = nullptr;

	float mfWidth = 0.0f;
	float mfHeight = 0.0f;

	anSound mTestSound;

	anTexture* mTest = nullptr;
};

int anStartApplication(char** args, int argc)
{
	Application* app = new Application();
	app->Start();

	return 0;
}
