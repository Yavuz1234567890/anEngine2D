#ifndef AN_EDITOR_STATE_H_
#define AN_EDITOR_STATE_H_

#include "State/anState.h"
#include "Scene/anScene.h"
#include "Device/anFramebuffer.h"
#include "Scene/anEntity.h"
#include "Scene/anSceneSerializer.h"
#include "anGizmoSystem.h"
#include "Core/anFileSystem.h"
#include "Scene/anSceneManager.h"
#include "Script/anNativeScript.h"

#include <ImGuiColorTextEdit/TextEditor.h>

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
	void LoadScene(const anString& path);
	anString GetExactTextEditorSource();
	anEntity& GetSelectedEntity();
	void RenderOverlays();
	void CloseProject();
	void DetectVSCode();
	void DetectVisualStudio();
	void ExecuteVSCodeCommand(const anString& cmd);
	void ExecuteVisualStudioCommand(const anString& cmd);
	void OpenFileWithVSCode(const anFileSystem::path& path);
	void OpenSolutionWithVisualStudio(const anFileSystem::path& path);
	void GenerateNativeScriptProjectPremake();
	void GenerateNativeScriptProject();
	void GenerateNativeScriptProjectFile();
	void CreateNativeScript(const anFileSystem::path& loc, const anString& name);
	void IncludeAllHeadersFromAssets(anOutputFile& file, anVector<anString>& names, const anFileSystem::path& path);
	static void ClearPath(anString& path);
	static void RemoveSpaces(anString& src);

	template<typename T>
	void DisplayAddComponentEntry(const anString& entryName);
private:
	const anString mNewEntityName = "Entity";
	anCamera2D mEditorCamera;
	anFileSystem::path mEditorScenePath;
	anEntity mSelectedEntity;

	anString mProjectName;
	anString mProjectSourceFileName;
	anFileSystem::path mProjectLocation;
	anFileSystem::path mProjectStartScenePath;
	anFileSystem::path mProjectAssetsLocation;
	anFileSystem::path mEditorPath;
	anFileSystem::path mEditorIncludePath;
	anFileSystem::path mEditorLibPath;
	anFileSystem::path mEditorBinPath;
	anFileSystem::path mProjectPremakeFile;
	anFileSystem::path mProjectSourceFile;
	anFileSystem::path mProjectSolution;

	anFramebuffer* mFramebuffer;

	float mfWidth;
	float mfHeight;

	anFloat2 mViewportSize;
	
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
	anTexture* mArrowIconTexture;

	anGizmoSystem mGizmoSystem;

	bool mNoScene = true;

	anFileSystem::path mAssetBrowserLocation;

	bool mLeftCtrl = false;
	bool mRightCtrl = false;

	bool mViewportWindowFocused = false;
	bool mViewportWindowHovered = false;
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

	bool mRenderPhysics = true;

	anFileSystem::path mVSCodePath;
	anFileSystem::path mVSCode;
	bool mVSCodeDetected = false;
	bool mOpenLuaFileWithVSCode = false;

	anFileSystem::path mVisualStudioPath;
	anFileSystem::path mVisualStudio;
	bool mVisualStudioDetected = false;
};

#endif
