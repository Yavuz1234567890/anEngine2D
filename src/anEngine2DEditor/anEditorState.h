#ifndef AN_EDITOR_STATE_H_
#define AN_EDITOR_STATE_H_

#include "State/anState.h"
#include "Scene/anScene.h"
#include "Device/anFramebuffer.h"
#include "Scene/anEntity.h"
#include "Scene/anSceneSerializer.h"
#include "anGizmoSystem.h"
#include "Core/anFileSystem.h"

#include <ImGuiColorTextEdit/TextEditor.h>

namespace anSceneState
{
	enum : anUInt32
	{
		Runtime,
		Edit
	};
}

namespace anLogType
{
	enum : anUInt32
	{
		Error,
		Info,
		Warning
	};
}

struct anLogData
{
	anUInt32 Type;
	anString From;
	anString Message;
};

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
	void OnAssetBrowserItemMove(const anFileSystem::path& oldPath, const anFileSystem::path& newPath);
	void MoveFileItem(const anFileSystem::path& fileName, const anFileSystem::path& newFolder);
	void OnAssetBrowserItemRemove(const anFileSystem::path& fileName);
	void SetStartingScene(const anFileSystem::path& path);
	void LoadScriptToTextEditor(const anFileSystem::path& path);
	void SaveTextEditorFile();
	bool IsTextEditorHaveFile();
	void CloseTextEditorScript();
	void EditorInfo(const anString& msg);
	void EditorError(const anString& msg);
	void EditorWarning(const anString& msg);
	void UserInfo(const anString& msg);
	void UserError(const anString& msg);
	void UserWarning(const anString& msg);
	void StartScene();
	void StopScene();
	void LoadScene(const anString& path);

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

	bool mViewportWindowFocused = false;
	bool mSceneWindowFocused = false;
	bool mEntityWindowFocused = false;
	bool mAssetBrowserWindowFocused = false;
	bool mTextEditorWindowFocused = false;

	TextEditor mTextEditor;
	anFileSystem::path mTextEditorCurrentFilePath;
	bool mIsTextEditorFileReadonly = false;
	bool mIsTextEditorFileSaved = false;
	anString mTextEditorFileSourceCode;

	anVector<anLogData> mLogs;
};

#endif
