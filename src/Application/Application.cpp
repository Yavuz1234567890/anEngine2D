#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Renderer/anRenderer.h"
#include "Device/anGPUCommands.h"
#include "World/anWorld.h"
#include "World/anObject.h"
#include "World/anSpriteObject.h"
#include "Core/anMessage.h"

class Application : public anApplication
{
public:
	Application()
		: anApplication({ "anEngine2D Application", 1200, 700 })
		, mScene(nullptr)
		, mSprite(nullptr)
		, mWorld(nullptr)
	{
	}

	~Application()
	{
	}

	void Initialize() override
	{
		mWorld = new anWorld();

		mRenderer.Initialize();

		const float w = (float)mApplicationDesc.Width;
		const float h = (float)mApplicationDesc.Height;

		mProjection = anMatrix4::Ortho(w * -0.5f, w * 0.5f, h * 0.5f, h * -0.5f, -1.0f, 1.0f);
		mRenderer.SetMatrix(mProjection);

		mScene = new anScene();

		mSprite = new anSpriteObject("test");
		mScene->AddObject(mSprite);

		mSprite->SetSize({ 20.0f, 20.0f });
		mSprite->SetTexture(anTexture::GetWhiteTexture());

		mWorld->SetCurrentScene(mScene);
		mWorld->Initialize();
	}

	void Update(float dt) override
	{
		mWorld->Update(dt);

		anClear();

		mRenderer.Start();

		mWorld->Render(mRenderer);

		mRenderer.End();
	}

private:
	anRenderer mRenderer;

	anMatrix4 mProjection;

	anScene* mScene;
	anSpriteObject* mSprite;
	anWorld* mWorld;
};

int anStartApplication(char** args, int argc)
{
	Application* app = new Application();
	app->Start();

	return 0;
}
