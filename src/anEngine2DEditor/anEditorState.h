#ifndef AN_EDITOR_STATE_H_
#define AN_EDITOR_STATE_H_

#include "State/anState.h"
#include "Scene/anScene.h"
#include "Device/anFramebuffer.h"
#include "Scene/anEntity.h"
#include "Scene/anSceneSerializer.h"
#include "anGizmoSystem.h"
#include "Core/anFileSystem.h"

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
	void SaveScene();
	void CloseScene();
	bool SceneIsValid() const;
	void OpenScene(const anFileSystem::path& path);
	static bool IsImageFile(const anString& extension);

	anEntity& GetSelectedEntity();

	template<typename T>
	void DisplayAddComponentEntry(const anString& entryName);
private:
	const anString mNewEntityName = "Entity";
	anScene* mEditorScene = nullptr;
	anScene* mRuntimeScene = nullptr;
	anCamera2D mEditorCamera;
	anFileSystem::path mEditorScenePath;
	anEntity mSelectedEntity;

	anString mProjectName;
	anFileSystem::path mProjectLocation;
	anFileSystem::path mProjectStartScenePath;
	anFileSystem::path mProjectAssetsLocation;

	anFramebuffer* mFramebuffer;

	float mfWidth;
	float mfHeight;

	anFloat2 mViewportSize;
	
	anUInt32 mSceneState = anSceneState::Edit;
	
	anFloat2 mViewportBounds[2];

	anFloat2 mMousePosition;
	anFloat2 mExactViewportMousePosition;
	anFloat2 mLastExactViewportMousePosition;
	anFloat2 mLastMousePosition;

	bool mDragEditorCamera = false;

	float mEditorCameraSpeed = 1.0f;

	anTexture* mPlayButtonTexture;
	anTexture* mStopButtonTexture;
	anTexture* mCameraIconTexture;
	anTexture* mFolderIconTexture;
	anTexture* mFileIconTexture;

	anGizmoSystem mGizmoSystem;

	anSceneSerializer mSceneSerializer;

	bool mNoScene = true;

	anFileSystem::path mAssetBrowserLocation;

	bool mCtrl = false;
};

#endif
