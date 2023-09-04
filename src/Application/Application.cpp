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
		mRaleway.Load("fonts/Raleway-Regular.ttf", 18);

		mWorld = new anWorld();

		mRenderer.Initialize();

		mfWidth = (float)mApplicationDesc.Width;
		mfHeight = (float)mApplicationDesc.Height;

		mProjection = anMatrix4::Ortho(mfWidth * -0.5f, mfWidth * 0.5f, mfHeight * 0.5f, mfHeight * -0.5f, -1.0f, 1.0f);
		mRenderer.SetMatrix(mProjection);

		mScene = new anScene();

		mSprite = new anSpriteObject("test");
		mSprite->SetSize({ 20.0f, 20.0f });
		mSprite->SetTexture(anTexture::GetWhiteTexture());
		mSprite->SetColor({ 0, 255, 255 });

		mText = new anTextObject("text");
		mText->SetPosition({ 0.0f, 10.0f });
		mText->SetText("Application Test");
		mText->SetFont(mRaleway);
		mText->SetColor({ 255, 0, 0 });
		
		mScene->AddObject(mText);
		mScene->AddObject(mSprite);

		mWorld->SetCurrentScene(mScene);
		mWorld->Initialize();
	}

	void Update(float dt) override
	{
		mWorld->Update(dt);

		if (mKeyW)
			mQuadPosition.Y -= 2.0f;

		if (mKeyS)
			mQuadPosition.Y += 2.0f;

		if (mKeyA)
			mQuadPosition.X -= 2.0f;

		if (mKeyD)
			mQuadPosition.X += 2.0f;

		mSprite->SetPosition(mQuadPosition);

		anClear();
		anEnableBlend();
		
		mRenderer.Start();

		mWorld->Render(mRenderer);

		const anString stats = "FPS: " + anToString(mFramesPerSecond) +
			"\nDraw Calls: " + anToString((int)mRenderer.GetDrawCallCount()) +
			"\nIndex Count: " + anToString((int)mRenderer.GetIndexCount());
		mRenderer.DrawString(mRaleway, { -mfWidth * 0.5f, -mfHeight * 0.5f + (float)mRaleway.GetSize() }, stats, { 255, 0, 255, 255 });

		mRenderer.End();
	}
	
	void OnEvent(const anEvent& event) override
	{
		if (event.Type == anEvent::KeyDown)
		{
			if (event.KeyCode == anKeyW)
				mKeyW = true;

			if (event.KeyCode == anKeyS)
				mKeyS = true;

			if (event.KeyCode == anKeyA)
				mKeyA = true;

			if (event.KeyCode == anKeyD)
				mKeyD = true;
		}

		if (event.Type == anEvent::KeyUp)
		{
			if (event.KeyCode == anKeyW)
				mKeyW = false;

			if (event.KeyCode == anKeyS)
				mKeyS = false;

			if (event.KeyCode == anKeyA)
				mKeyA = false;

			if (event.KeyCode == anKeyD)
				mKeyD = false;
		}
	}

private:
	anRenderer mRenderer;

	anMatrix4 mProjection;

	anFont mRaleway;

	anScene* mScene = nullptr;
	anTextObject* mText = nullptr;
	anWorld* mWorld = nullptr;

	anSpriteObject* mSprite = nullptr;
	anFloat2 mQuadPosition;

	float mfWidth = 0.0f;
	float mfHeight = 0.0f;

	bool mKeyW = false;
	bool mKeyS = false;
	bool mKeyA = false;
	bool mKeyD = false;
};

int anStartApplication(char** args, int argc)
{
	Application* app = new Application();
	app->Start();

	return 0;
}
