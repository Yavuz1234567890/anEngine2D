#ifndef AN_STATE_MANAGER_H_
#define AN_STATE_MANAGER_H_

#include "anState.h"

class anStateManager
{
public:
	anStateManager(anApplication* app);
	~anStateManager();

	void SetCurrentState(anState* state);
	anState* GetCurrentState();
	anApplication* GetApplication();

	template<class T>
	void SetCurrentState()
	{
		SetCurrentState(new T(mApplication));
	}

	void Initialize();
	void Update(float dt);
	void OnEvent(const anEvent& event);
	void Render(anRenderer& renderer);
private:
	anState* mCurrentState = nullptr;
	anApplication* mApplication = nullptr;
};

#endif
