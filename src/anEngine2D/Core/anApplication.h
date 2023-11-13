#ifndef AN_APPLICATION_H_
#define AN_APPLICATION_H_

#include "anWindow.h"
#include "anTimer.h"
#include "anEvent.h"
#include "Renderer/anRenderer.h"
#include "Core/anSound.h"

class anStateManager;
class anState;

struct anApplicationCreationDescription
{
	anString Title;
	int Width;
	int Height;
	bool WindowResizable;
};

class anApplication
{
public:
	anApplication(const anApplicationCreationDescription& desc);
	~anApplication();

	virtual void Initialize() = 0;
	virtual void Update(float dt) = 0;
	virtual void OnEvent(const anEvent& event) = 0;

	void Start();
	void AOnEvent(const anEvent& event);
	void Render(anRenderer& renderer);
	anWindow* GetWindow();
	void LogWrite(const anString& msg);
protected:
	void SetCurrentState(anState* state);
	anState* GetCurrentState();

	template<class T>
	void SetCurrentState()
	{
		SetCurrentState(new T(this));
	}

protected:
	anApplicationCreationDescription mApplicationDesc;
	anWindow* mWindow;
	anTimer mTimer;
	int mFramesPerSecond;
	anStateManager* mStateManager;
	anOutputFile mLogFile;
};

#endif
