#ifndef AN_STATE_H_
#define AN_STATE_H_

#include "Core/anEvent.h"
#include "Core/anApplication.h"
#include "Renderer/anRenderer2D.h"

class anState
{
public:
	anState(anApplication* app);
	~anState();

	virtual void Initialize() = 0;
	virtual void Update(float dt) = 0;
	virtual void OnEvent(const anEvent& event) = 0;
	virtual void Render2D(anRenderer2D& renderer) = 0;
	virtual void OnImGuiRender() = 0;

	anApplication* GetApplication();
protected:
	anApplication* mApplication = nullptr;
};

#endif
