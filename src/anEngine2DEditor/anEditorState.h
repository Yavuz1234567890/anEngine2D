#ifndef AN_EDITOR_STATE_H_
#define AN_EDITOR_STATE_H_

#include "State/anState.h"
#include "Scene/anScene.h"
#include "Device/anFramebuffer.h"
#include "Scene/anEntity.h"
#include "Scene/anSceneSerializer.h"
#include "anGizmoSystem.h"

namespace anSceneState
{
	enum : anUInt32
	{
		Runtime,
		Edit
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
	void DrawToolbar();
	void SaveSceneAs();
	void NewScene();
	void SaveScene();
	
	anEntity& GetSelectedEntity();

	template<typename T>
	void DisplayAddComponentEntry(const anString& entryName);
private:
	anScene* mEditorScene;
	anCamera2D mEditorCamera;
	anString mEditorScenePath;
	anEntity mSelectedEntity;

	anString mProjectName;
	anString mProjectLocation;
	anString mProjectStartScenePath;
	anString mProjectScenesLocation;
	anString mProjectAssetsLocation;

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

	bool mDragEditorCamera = false;

	float mEditorCameraSpeed = 1.0f;

	anTexture* mPlayButtonTexture;
	anTexture* mStopButtonTexture;
	anTexture* mCameraIconTexture;

	anGizmoSystem mGizmoSystem;

	anSceneSerializer mSceneSerializer;

	bool mFreshScene = true;
};

#endif
