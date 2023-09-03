#include "anApplication.h"
#include "Device/anShaders.h"
#include "Device/anTexture.h"

anApplication::anApplication(const anApplicationCreationDescription& desc)
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
	mWindow = anCreateWindow(mApplicationDesc.Title, mApplicationDesc.Width, mApplicationDesc.Height);
	anTexture::Initialize();
	anInitializeShaders();
	Initialize();

	while (mWindow->IsRunning())
	{
		const float dt = mTimer.Tick();
		Update(dt);
		mWindow->Present();
	}
}

anWindow* anApplication::GetWindow()
{
	return mWindow;
}
