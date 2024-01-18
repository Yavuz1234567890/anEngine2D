#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Core/anKeyCodes.h"
#include "State/anStateManager.h"
#include "Math/anMath.h"
#include "anGameState.h"

class anEngine2DApplication : public anApplication
{
public:
	anEngine2DApplication()
		: anApplication({ "anEngine2D Application", 1200, 700, false, false })
	{
	}

	~anEngine2DApplication()
	{
	}

	void Initialize() override
	{
		anInitializeRandomDevice();

		mWindow->MakeFullscreen();

		SetCurrentState<anGameState>();
	}

	void Update(float dt) override
	{
	}

	void OnEvent(const anEvent& event) override
	{
	}

	void OnImGuiRender() override
	{
	}
};

int anStartApplication(char** args, int argc)
{
	anEngine2DApplication* app = new anEngine2DApplication();
	app->Start();

	return 0;
}

