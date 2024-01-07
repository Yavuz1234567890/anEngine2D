#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Core/anKeyCodes.h"
#include "State/anStateManager.h"
#include "Math/anMath.h"
#include "anGameState.h"

class anEngine2DEditorApplication : public anApplication
{
public:
	anEngine2DEditorApplication()
		: anApplication({ "anEngine2D Application", 1200, 700, false, false })
	{
	}

	~anEngine2DEditorApplication()
	{
	}

	void Initialize() override
	{
		anInitializeRandomDevice();

		mWindow->SetVSync(true);
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
	anEngine2DEditorApplication* app = new anEngine2DEditorApplication();
	app->Start();

	return 0;
}

