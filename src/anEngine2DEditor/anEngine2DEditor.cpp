#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Core/anKeyCodes.h"
#include "State/anStateManager.h"
#include "Math/anMath.h"
#include "anEditorState.h"

class anEngine2DEditorApplication : public anApplication
{
public:
	anEngine2DEditorApplication()
		: anApplication({ "anEngine2D Editor", 1200, 700, true, true })
	{
	}

	~anEngine2DEditorApplication()
	{
	}

	void Initialize() override
	{
		anInitializeRandomDevice();

		mWindow->SetVSync(true);

		SetCurrentState<anEditorState>();
	}

	void Update(float dt) override
	{
	}

	void OnEvent(const anEvent& event) override
	{
		if (event.Type == anEvent::KeyDown)
		{
			if (event.KeyCode == anKeyEscape)
				mWindow->Close();
		}
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

