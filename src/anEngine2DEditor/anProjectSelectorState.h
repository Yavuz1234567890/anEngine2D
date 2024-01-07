#ifndef AN_PROJECT_SELECTOR_STATE_H_
#define AN_PROJECT_SELECTOR_STATE_H_

#include "State/anState.h"

class anProjectSelectorState : public anState
{
public:
	anProjectSelectorState(anApplication* app);
	~anProjectSelectorState();

	void Initialize() override;
	void Update(float dt) override;
	void OnEvent(const anEvent& event) override;
	void OnImGuiRender() override;
private:
	anString mProjectFolder;
	anVector<const char*> mProjectExtension = { ".anProj" };
	bool mCreateProjectFolder = true;
};

#endif
