#ifndef TEST_STATE_H_
#define TEST_STATE_H_

#include "State/anState.h"

class TestState : public anState
{
public:
	TestState(anApplication* app);
	~TestState();

	void Initialize() override;
	void Update(float dt) override;
	void OnEvent(const anEvent& event) override;
	void Render2D(anRenderer2D& renderer) override;
	void OnImGuiRender() override;
};

#endif
