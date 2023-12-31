#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Renderer/anRenderer2D.h"
#include "Device/anGPUCommands.h"
#include "Core/anMessage.h"
#include "Core/anKeyCodes.h"
#include "State/anStateManager.h"
#include "Math/anMath.h"
#include "TestState.h"
#include "Renderer/anParticleSystem2D.h"
#include "Device/anFramebuffer.h"
#include "Scene/anEntity.h"

#include <imgui/imgui.h>

class Application : public anApplication
{
public:
	Application()
		: anApplication({ "anEngine2D Application", 1200, 700, true, false })
	{
	}

	~Application()
	{
	}

	void Initialize() override
	{
		anInitializeRandomDevice();

		mWindow->SetVSync(true);
		
		mTest = anLoadTexture("assets/test.png");

		mRaleway.Load("fonts/raleway/Raleway-Regular.ttf", 60);
		
		mfWidth = 1200.0f;
		mfHeight = 700.0f;

		mCamera.SetOrtho(-mfWidth * 0.5f, mfWidth * 0.5f, -mfHeight * 0.5f, mfHeight * 0.5f);
		
		mFramebuffer = new anFramebuffer({ (anUInt32)mfWidth, (anUInt32)mfHeight });

		SetCurrentState<TestState>();

		mTestScene = new anScene();

		{
			mTestEntity = mTestScene->NewEntity("test");
			auto& sprite = mTestEntity.AddComponent<anSpriteRendererComponent>();
			sprite.Texture = mTest;
			auto& transform = mTestEntity.GetComponent<anTransformComponent>();
			transform.Size = { 400.0f, 200.0f };
		}

		{
			mCameraEntity = mTestScene->NewEntity("camera");
			auto& camera = mCameraEntity.AddComponent<anCameraComponent>();
			camera.Camera.SetOrtho(-mfWidth * 0.5f, mfWidth * 0.5f, -mfHeight * 0.5f, mfHeight * 0.5f);
		}

		mTestScene->RuntimeInitialize();
	}

	void Update(float dt) override
	{
		anClear();
		anEnableBlend();

		mFramebuffer->Bind();

		anClear();
		anEnableBlend();

		mTestScene->RuntimeUpdate(dt);

		mFramebuffer->Unbind();
	}
	
	void OnEvent(const anEvent& event) override
	{
		if (event.Type == anEvent::KeyDown)
		{
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
	anMatrix4 mProjection;

	anFont mRaleway;

	float mfWidth = 0.0f;
	float mfHeight = 0.0f;

	anTexture* mTest = nullptr;

	anFloat2 mTexturePos = { 0.0f, 0.0f };

	anCamera2D mCamera;

	anFloat2 mLastMousePosition;
	anFloat2 mMousePosition;
	bool mDragMouse = false;

	anParticleSystem2D mParticleSystem;

	anFramebuffer* mFramebuffer;

	anEntity mTestEntity;
	anEntity mCameraEntity;
	anScene* mTestScene;
};

int anStartApplication(char** args, int argc)
{
	Application* app = new Application();
	app->Start();

	return 0;
}
