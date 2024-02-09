#ifndef AN_GAME_STATE_H_
#define AN_GAME_STATE_H_

#define EDITOR

#include "State/anState.h"
#include "Scene/anScene.h"
#include "Scene/anSceneSerializer.h"

class anGameState : public anState
{
public:
	anGameState(anApplication* app);
	~anGameState();

	void Initialize() override;
	void Update(float dt) override;
	void OnEvent(const anEvent& event) override;
	void OnImGuiRender() override;
private:
	anFileSystem::path mCurrentPath;
	anFileSystem::path mAssetsPath;

	float mfWidth = 0.0f;
	float mfHeight = 0.0f;
};

#endif
