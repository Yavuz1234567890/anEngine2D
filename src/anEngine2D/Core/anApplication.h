#ifndef AN_APPLICATION_H_
#define AN_APPLICATION_H_

#include "anWindow.h"
#include "anTimer.h"
#include "anEvent.h"

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
	anWindow* GetWindow();
protected:
	anApplicationCreationDescription mApplicationDesc;
	anWindow* mWindow;
	anTimer mTimer;
	int mFramesPerSecond;
};

#endif
