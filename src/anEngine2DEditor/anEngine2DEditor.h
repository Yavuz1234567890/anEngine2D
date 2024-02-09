#ifndef AN_ENGINE2D_EDITOR_H_
#define AN_ENGINE2D_EDITOR_H_

#include "Core/anApplication.h"
#include "Core/anEntryPoint.h"
#include "Core/anKeyCodes.h"
#include "State/anStateManager.h"
#include "Math/anMath.h"
#include "anProjectSelectorState.h"
#include "Core/anMessage.h"
#include "Core/anFileSystem.h"

class anEngine2DApplication : public anApplication
{
public:
	anEngine2DApplication();
	~anEngine2DApplication();

	void PrepareImGuiStyle();
	void Initialize() override;
	void Update(float dt) override;
	void OnEvent(const anEvent& event) override;
	void OnImGuiRender() override;
};

#endif
