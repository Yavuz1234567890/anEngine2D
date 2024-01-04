#ifndef AN_EDITOR_STATE_H_
#define AN_EDITOR_STATE_H_

#include "State/anState.h"
#include "Scene/anScene.h"
#include "Device/anFramebuffer.h"
#include "Scene/anEntity.h"

namespace anSceneState
{
	enum : anUInt32
	{
		Runtime,
		Edit
	};
}

namespace anGizmoType
{
	enum : anUInt32
	{
		Translate,
		Scale,
		Rotate
	};
}

class anEditorState : public anState
{
public:
	anEditorState(anApplication* app);
	~anEditorState();

	void Initialize() override;
	void Update(float dt) override;
	void OnEvent(const anEvent& event) override;
	void OnImGuiRender() override;
	void BeginImGuiDockspace();
	void EndImGuiDockspace();
	void OnScenePanel();
	void DrawEntityToScenePanel(anEntity entity);
	void OnEntityPanel();
	void DrawComponents(anEntity entity);
	void UpdateGizmos(float dt);
	void DrawToolbar();
	
	template<typename T>
	void DisplayAddComponentEntry(const anString& entryName);
private:
	anScene* mEditorScene;
	anCamera2D mEditorCamera;
	anEntity mSelectedEntity;

	anFramebuffer* mFramebuffer;

	float mfWidth;
	float mfHeight;

	anFloat2 mViewportSize;
	
	anUInt32 mSceneState = anSceneState::Edit;

	anFloat2 mViewportBounds[2];

	anFloat2 mViewportMousePosition;
	anFloat2 mExactViewportMousePosition;
	anFloat2 mLastExactViewportMousePosition;
	anFloat2 mViewportLastMousePosition;

	anFloat2 mLeftMouseTickedViewportMousePosition;
	anFloat2 mLeftMouseTickedGizmoPosition;
	
	bool mDragEditorCamera = false;
	bool mLeftMouseButtonPressed = false;

	float mEditorCameraSpeed = 1.0f;

	anUInt32 mGizmoType = -1;
	anFloat2 mGizmoPosition;
	bool mGizmoHorizontalTick = false;
	bool mGizmoVerticalTick = false;
	bool mGizmoIsUsing = false;
	float mGizmoSize = 100.0f;
	float mGizmoRectSize = mGizmoSize * 0.2f;
	anFloat2 mGizmoBoxPosition;
	bool mGizmoBoxTick = false;

	anTexture* mPlayButtonTexture;
	anTexture* mStopButtonTexture;
	anTexture* mCameraIconTexture;
};

#endif
