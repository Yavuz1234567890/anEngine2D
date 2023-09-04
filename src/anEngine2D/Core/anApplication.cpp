#include "anApplication.h"
#include "Device/anShaders.h"
#include "Device/anTexture.h"
#include "anFont.h"

anApplication::anApplication(const anApplicationCreationDescription& desc)
	: mFramesPerSecond(0)
{
	memset(&mApplicationDesc, 0, sizeof(anApplicationCreationDescription));
	mApplicationDesc.Title = desc.Title;
	mApplicationDesc.Width = desc.Width;
	mApplicationDesc.Height = desc.Height;
}

anApplication::~anApplication()
{
}

void anApplication::Start()
{
	auto onEvent = [this](const anEvent& event) { OnEvent(event); };

	mWindow = anCreateWindow(mApplicationDesc.Title, mApplicationDesc.Width, mApplicationDesc.Height, onEvent);
	anTexture::Initialize();
	anFont::Initialize();
	anInitializeShaders();
	Initialize();

	int fps = 0;
	float elapsedTime = 0.0f;
	while (mWindow->IsRunning())
	{
		const float dt = mTimer.Tick();
		elapsedTime += dt;
		if (elapsedTime >= 1.0f)
		{
			mFramesPerSecond = fps;
			elapsedTime = 0.0f;
			fps = 0;
		}
		++fps;

		Update(dt);
		mWindow->Present();
	}
}

anWindow* anApplication::GetWindow()
{
	return mWindow;
}
