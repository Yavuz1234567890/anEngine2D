#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Renderer/anRenderer2D.h"
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
		mWindow->SetVSync(true);
		mWindow->MakeFullscreen();

		mTest = anLoadTexture("assets/test.png");

		mRaleway.Load("assets/Raleway-Regular.ttf", 18);
		mTestSound.Load("assets/jaguar.wav");

		mWorld = new anWorld();

		mRenderer.Initialize();
		
		anFloat2 monitorSize = mWindow->GetMonitorSize();
		mfWidth = monitorSize.x;
		mfHeight = monitorSize.y;

		mProjection = glm::ortho(mfWidth * -0.5f, mfWidth * 0.5f, mfHeight * 0.5f, mfHeight * -0.5f, -1.0f, 1.0f);
		mRenderer.SetMatrix(mProjection);

		mWorld->Initialize();

		SetCurrentState<TestState>();
	}

	void Update(float dt) override
	{
		mWorld->Update(dt);
		
		anController controller = mControllerDevice.GetController(0);
		if (controller.IsConnected)
			mTexturePos += anFloat2(controller.LeftAxis.x, -controller.LeftAxis.y) * 3.0f;
	
		anClearColor({ 255, 0, 0 });
		anEnableBlend();

		mRenderer.Start();

		anApplication::Render(mRenderer);
		mWorld->Render(mRenderer);

		mRenderer.DrawTexture(mTest, mTexturePos, { 642.0f, 313.0f }, 0.0f, { 255, 255, 255 });
		mRenderer.DrawString(mRaleway, { 100.0f, 100.0f }, "FPS: " + anToString(mFramesPerSecond), { 255, 0, 255, 255 });
		mRenderer.DrawString(mRaleway, { 0.0f, 400.0f }, "Press ESC to exit", { 255, 255, 255 });

		mRenderer.End();
	}
	
	void OnEvent(const anEvent& event) override
	{
		if (event.Type == anEvent::KeyDown)
		{
			if (event.KeyCode == anKeySpace)
				mTestSound.Play();
		
			if (event.KeyCode == anKeyEscape)
				mWindow->Close();
		}
	}

	void OnImGuiRender() override
	{
	}

private:
	anRenderer2D mRenderer;

	anMatrix4 mProjection;

	anFont mRaleway;

	anScene* mScene = nullptr;
	anWorld* mWorld = nullptr;

	float mfWidth = 0.0f;
	float mfHeight = 0.0f;

	anSound mTestSound;

	anTexture* mTest = nullptr;

	anFloat2 mTexturePos;
};

int anStartApplication(char** args, int argc)
{
	Application* app = new Application();
	app->Start();

	return 0;
}
