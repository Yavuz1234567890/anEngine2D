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
#include "Renderer/anParticleSystem2D.h"
#include "Device/anFramebuffer.h"

#include <imgui/imgui.h>

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

		mRaleway.Load("fonts/raleway/Raleway-Regular.ttf", 60);
		mTestSound.Load("assets/jaguar.wav");

		mWorld = new anWorld();

		mRenderer.Initialize();
		
		anFloat2 monitorSize = mWindow->GetMonitorSize();
		mfWidth = monitorSize.x;
		mfHeight = monitorSize.y;

		mCamera.SetOrtho(-mfWidth * 0.5f, mfWidth * 0.5f, -mfHeight * 0.5f, mfHeight * 0.5f);
		
		mWorld->Initialize();

		mFramebuffer = new anFramebuffer({ (anUInt32)mfWidth, (anUInt32)mfHeight });

		SetCurrentState<TestState>();
	}

	void Update(float dt) override
	{
		mWorld->Update(dt);
		mParticleSystem.Update(dt);
		
		anParticle2DProps props;
		props.Position = { 0.0f, 0.0f };
		props.LifeTime = 1.0f;
		props.ColorBegin = { 255, 255, 0 };
		props.ColorEnd = { 255, 0, 0 };
		props.SizeBegin = 30.0f;
		props.SizeEnd = 0.0f;
		props.VelocityXInterval = { -1.0f, 1.0f };
		props.VelocityYInterval = { 0.0f, -1.0f };
		props.Velocity = { 30.0f, 100.0f };
		mParticleSystem.Add(props);

		anController controller = mControllerDevice.GetController(0);
		if (controller.IsConnected)
			mTexturePos += anFloat2(controller.LeftAxis.x, -controller.LeftAxis.y) * 3.0f;

		anClear();
		anEnableBlend();

		mFramebuffer->Bind();

		mRenderer.Start(mCamera);

		anApplication::Render2D(mRenderer);
		mWorld->Render(mRenderer);

		mRenderer.DrawTexture(mTest, mTexturePos, { 642.0f, 313.0f }, 0.0f, {255, 255, 255});
		mRenderer.DrawString(mRaleway, { 100.0f, 100.0f }, "FPS: " + anToString(mFramesPerSecond), { 255, 0, 255, 255 });
		mRenderer.DrawString(mRaleway, { 0.0f, 400.0f }, "Press ESC to exit", { 255, 255, 255 });

		mParticleSystem.Render2D(mRenderer);

		mRenderer.End();

		mFramebuffer->Unbind();
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

		if (event.Type == anEvent::MouseDown)
		{
			if (event.MouseButton == 1)
				mDragMouse = true;
		}

		if (event.Type == anEvent::MouseUp)
		{
			if (event.MouseButton == 1)
				mDragMouse = false;
		}

		if (event.Type == anEvent::MouseMove)
		{
			mMousePosition = event.MousePosition;
			if (mDragMouse)
				mCamera.Move((mLastMousePosition - mMousePosition) * mCamera.GetZoomLevel());

			mLastMousePosition = mMousePosition;
		}

		if (event.Type == anEvent::MouseWheel)
		{
			mCamera.IncreaseZoomLevel(-event.MouseScroll.y / 10.0f);
			mCamera.Move((mMousePosition - anFloat2(mfWidth, mfHeight) * 0.5f) / 10.0f * event.MouseScroll.y / anAbs(-event.MouseScroll.y));
		}
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Screen");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		
		ImGui::Image(reinterpret_cast<void*>(mFramebuffer->GetTextureID()), { viewportPanelSize.x, viewportPanelSize.y }, { 0, 1 }, { 1, 0 });
		ImGui::End();
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

	anFloat2 mTexturePos = { 0.0f, 0.0f };

	anCamera2D mCamera;

	anFloat2 mLastMousePosition;
	anFloat2 mMousePosition;
	bool mDragMouse = false;

	anParticleSystem2D mParticleSystem;

	anFramebuffer* mFramebuffer;
};

int anStartApplication(char** args, int argc)
{
	Application* app = new Application();
	app->Start();

	return 0;
}
