#ifndef AN_APPLICATION_H_
#define AN_APPLICATION_H_

#include "anWindow.h"
#include "anTimer.h"
#include "anEvent.h"
#include "Renderer/anRenderer2D.h"
#include "Core/anSound.h"
#include "anControllerDevice.h"
#include "ImGui/anImGuiDevice.h"
#include "Script/anScriptSystem.h"

class anStateManager;
class anState;

struct anApplicationCreationDescription
{
	anString Title;
	int Width;
	int Height;
	bool WindowResizable;
	bool WindowMaximized;
};

class anApplication
{
public:
	anApplication(const anApplicationCreationDescription& desc);
	~anApplication();

	virtual void Initialize() = 0;
	virtual void Update(float dt) = 0;
	virtual void OnEvent(const anEvent& event) = 0;
	virtual void OnImGuiRender() = 0;

	void Start();
	void AOnEvent(const anEvent& event);
	void OnImGui();
	anWindow* GetWindow();
	void LogWrite(const anString& msg);
	anControllerDevice GetControllerDevice();
	int GetFramesPerSecond() const;
	void SetCurrentState(anState* state);
	anState* GetCurrentState();

	template<class T>
	void SetCurrentState()
	{
		SetCurrentState(new T(this));
	}

private:
	anImGuiDevice mImGui;
protected:
	anApplicationCreationDescription mApplicationDesc;
	anWindow* mWindow;
	anTimer mTimer;
	int mFramesPerSecond;
	anStateManager* mStateManager;
	anOutputFile mLogFile;
	anControllerDevice mControllerDevice;
};

#endif
