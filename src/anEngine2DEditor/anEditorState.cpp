#include "anEditorState.h"
#include "Device/anGPUCommands.h"
#include "Core/anKeyCodes.h"
#include "Math/anMath.h"
#include "Project/anProject.h"
#include "Core/anMessage.h"
#include "Core/anInputSystem.h"
#include "anProjectSelectorState.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

static bool ImGuiColorPicker(const char* label, anColor& in, bool use = true)
{
	float colors[4];
	colors[0] = float(in.R) / 255.0f;
	colors[1] = float(in.G) / 255.0f;
	colors[2] = float(in.B) / 255.0f;
	colors[3] = float(in.A) / 255.0f;

	if (!ImGui::ColorPicker4(label, colors))
		return false;

	if (use)
	{
		in.R = int(colors[0] * 255.0f);
		in.G = int(colors[1] * 255.0f);
		in.B = int(colors[2] * 255.0f);
		in.A = int(colors[3] * 255.0f);
	}

	return true;
}

static anString LogTypeToString(anUInt32 type)
{
	switch (type)
	{
	case anLogType::Error: return "Error";
	case anLogType::Info: return "Info";
	case anLogType::Warning: return "Warning";
	}

	return "";
}

anEditorState::anEditorState(anApplication* app)
	: anState(app)
{
}

anEditorState::~anEditorState()
{
}

void anEditorState::Initialize()
{
	{
		auto editorInfo = [this](const anString& msg) { EditorInfo(msg); };
		auto editorError = [this](const anString& msg) { EditorError(msg); };
		auto editorWarning = [this](const anString& msg) { EditorWarning(msg); };

		auto userInfo = [this](const anString& msg) { UserInfo(msg); };
		auto userError = [this](const anString& msg) { UserError(msg); };
		auto userWarning = [this](const anString& msg) { UserWarning(msg); };

		anSetEditorLogCallback({ editorInfo, editorError, editorWarning });
		anSetEditorUserLogCallback({ userInfo, userError, userWarning });
	}

	mPlayButtonTexture = anLoadTexture("icons/playbutton.png");
	mCameraIconTexture = anLoadTexture("icons/cameraicon.png");
	mFolderIconTexture = anLoadTexture("icons/directoryicon.png");
	mFileIconTexture = anLoadTexture("icons/fileicon.png");
	mArrowIconTexture = anLoadTexture("icons/arrowicon.png");

	if (!anProjectManager::GetCurrentProject())
		mApplication->GetWindow()->Close();

	mProjectLocation = anProjectManager::GetCurrentProject()->Location;
	mProjectName = anProjectManager::GetCurrentProject()->Name;
	mProjectSourceFileName = mProjectName + ".cpp";

	mEditorPath = anFileSystem::current_path();
	mEditorIncludePath = mEditorPath / "include";
	mEditorLibPath = mEditorPath / "lib";
	mEditorBinPath = mEditorPath / "bin";
	mProjectPremakeFile = mProjectLocation / "premake5.lua";
	mProjectSourceFile = mProjectLocation / mProjectSourceFileName;
	mProjectSolution = mProjectLocation / (mProjectName + ".sln");
	mProjectAssetsLocation = mProjectLocation / "assets";
	mProjectBuildFile = mProjectLocation / "build.bat";

	mAssetBrowserLocation = mProjectAssetsLocation;
											   
	mProjectStartScenePath = mProjectLocation / anProjectManager::GetCurrentProject()->StartScene;

	if (!anProjectManager::GetCurrentProject()->StartScene.empty() && anFileSystem::exists(mProjectStartScenePath))
	{
		mEditorScenePath = mProjectStartScenePath;
		anSceneManager::Load(mProjectLocation.string(), mProjectStartScenePath.string());
		mNoScene = false;
	}

	mApplication->GetWindow()->SetTitle("anEngine2D Editor - " + mProjectName + " - " + (mNoScene ? "No Scene" : anFileSystem::path{mEditorScenePath}.filename().string()));

	DetectVSCode();
	DetectVisualStudio();
	
	GenerateBuildFile();
	anFileSystem::create_directory(mProjectAssetsLocation);
	GenerateNativeScriptProjectPremake();
	GenerateNativeScriptProject();
	BuildSolution();
	OpenSolutionWithVisualStudio(mProjectSolution);

	mFramebuffer = new anFramebuffer({ mApplication->GetWindow()->GetStartWidth(), mApplication->GetWindow()->GetStartHeight() });

	{
		const int iWidth = (int)mApplication->GetWindow()->GetStartWidth();
		const int iHeight = (int)mApplication->GetWindow()->GetStartHeight();

		mfWidth = (float)iWidth;
		mfHeight = (float)iHeight;
	}

	mGizmoSystem.Initialize(this);

	mTextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	mTextEditor.SetHandleKeyboardInputs(false);
	mTextEditor.SetHandleMouseInputs(false);

	anEditorInfo("Engine initialized");
	anEditorInfo("Welcome to anEngine2D");
}

void anEditorState::Update(float dt)
{
	mProjectBuilded = anFileSystem::exists(mProjectLocation / (mProjectName + ".exe"));

	if (mRunProject && mProjectBuilded)
	{
		const anFileSystem::path prev = anFileSystem::current_path();
		anFileSystem::current_path(mProjectLocation);
		anString scenePath = mEditorScenePath.string();
		ClearPath(scenePath, '\\');
		system(("call \"" + mProjectName + ".exe\" \"" + scenePath + "\"").c_str());
		anFileSystem::current_path(prev);
		
		mRunProject = false;
	}

	if (SceneIsValid())
		anSceneManager::Get()->OnViewportSize(anUInt32(mViewportSize.x), anUInt32(mViewportSize.y));

	anFramebufferSpecification spec = mFramebuffer->GetSpecification();
	if (mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && (spec.Width != mViewportSize.x || spec.Height != mViewportSize.y))
	{
		mFramebuffer->Resize((anUInt32)mViewportSize.x, (uint32_t)mViewportSize.y);
		mEditorCamera.SetOrtho(mViewportSize.x * -0.5f, mViewportSize.x * 0.5f, mViewportSize.y * -0.5f, mViewportSize.y * 0.5f);
	}

	anClear();
	
	anRenderer2D::Get().ResetStats();
	mFramebuffer->Bind();

	anEnableBlend();
	anClearColor(SceneIsValid() ? anSceneManager::Get()->GetClearColor() : anColor(0, 0, 0));

	auto [mx, my] = ImGui::GetMousePos();
	mx -= mViewportBounds[0].x;
	my -= mViewportBounds[0].y;
	anFloat2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
	my = viewportSize.y - my;
	mMousePosition = { (float)mx, (float)my };
	mLastExactViewportMousePosition = mExactViewportMousePosition;

	anFloat2 camPos = { 0.0f, 0.0f };
	if (SceneIsValid())
	{
		if (anSceneManager::Get()->HasCamera())
			camPos = anSceneManager::Get()->GetCurrentCameraPosition();
	}
	
	mExactViewportMousePosition = ((mMousePosition - (mViewportBounds[1] - mViewportBounds[0]) * 0.5f) * anFloat2(1.0f, -1.0f)) + camPos;

	if (SceneIsValid())
	{
		anSceneManager::Get()->EditorUpdate(dt, mEditorCamera);

		RenderOverlays();
		mGizmoSystem.UpdateGizmos(dt, mExactViewportMousePosition);

		anRenderer2D::Get().End();

		if (mMousePosition.x >= 0 && mMousePosition.y >= 0 && mMousePosition.x < viewportSize.x && mMousePosition.y < viewportSize.y && mDragEditorCamera && !mGizmoSystem.IsUsing())
			mEditorCamera.Move((mLastMousePosition - mMousePosition) * mEditorCameraSpeed * anFloat2(1.0f, -1.0f));
	}

	mLastMousePosition = mMousePosition;

	mFramebuffer->Unbind();
}

void anEditorState::OnEvent(const anEvent& event)
{
	if (event.Type == anEvent::Drop)
	{
		for (const auto path : event.DropedFiles)
			anFileSystem::copy_file(path, mAssetBrowserLocation / path.filename());
	}

	if (event.Type == anEvent::KeyDown)
	{
		if (event.KeyCode == anKeyLeftControl)
			mLeftCtrl = true;

		if (event.KeyCode == anKeyRightControl)
			mRightCtrl = true;
	}

	if (event.Type == anEvent::KeyUp)
	{
		if (event.KeyCode == anKeyLeftControl)
			mLeftCtrl = false;

		if (event.KeyCode == anKeyRightControl)
			mRightCtrl = false;
	}

	if (SceneIsValid())
	{
		mGizmoSystem.OnEvent(event);

		if (event.Type == anEvent::KeyDown)
		{
			if (event.KeyCode == anKeyS)
			{
				if (mLeftCtrl || mRightCtrl)
				{
					if (mTextEditor.IsTextBoxFocused())
						SaveTextEditorFile();
					else
						SaveScene();
				}
			}

			if (event.KeyCode == anKeyW)
			{
				if (mLeftCtrl || mRightCtrl)
				{
					if (mTextEditor.IsTextBoxFocused())
						CloseTextEditorScript();
					else
						CloseScene();
				}
			}

			if (event.KeyCode == anKeyB)
			{
				if (mLeftCtrl || mRightCtrl)
					BuildSolution();
			}

			if (event.KeyCode == anKeyR)
			{
				if (mLeftCtrl || mRightCtrl)
					anSceneManager::Get()->ReloadAssets();
			}

			if (event.KeyCode == anKeyDelete)
			{
				if (GImGui->ActiveId == 0 && (mSceneWindowFocused || mViewportWindowFocused || mEntityWindowFocused))
				{
					anEntity ent = mSelectedEntity;
					if (ent.GetHandle() != entt::null)
					{
						anSceneManager::Get()->DestroyEntity(ent);
						mSelectedEntity = {};
					}
				}
			}
		}
	}

	if (event.Type == anEvent::MouseDown)
	{
		if (event.MouseButton == 1)
			mDragEditorCamera = true;
	}

	if (event.Type == anEvent::MouseUp)
	{
		if (event.MouseButton == 1)
			mDragEditorCamera = false;
	}
}

void anEditorState::OnImGuiRender()
{
	BeginImGuiDockspace();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Application"))
		{
			if (ImGui::MenuItem("Exit"))
				mApplication->GetWindow()->Close();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Project"))
		{
			if (ImGui::MenuItem("Close"))
				CloseProject();

			if (ImGui::MenuItem("Set Starting Scene"))
			{
				if (mNoScene)
					anShowMessageBox("Error", "The current scene is fresh scene", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);
				else
					SetStartingScene(mEditorScenePath.lexically_relative(mProjectLocation));
			}

			if (ImGui::MenuItem("Export"))
				ExportProject();

			if (ImGui::MenuItem("Build", "Ctrl+B"))
				BuildSolution();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				SaveScene();

			if (ImGui::MenuItem("Close Scene", "Ctrl+W"))
				CloseScene();

			if (ImGui::MenuItem("Reload Assets", "Ctrl+R"))
				anSceneManager::Get()->ReloadAssets();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	
	ImGui::Begin("Options");

	if (SceneIsValid())
	{
		ImGui::Text(mProjectBuilded ? "Project Builded" : "Project Unbuilded");
		ImGui::Separator();
		ImGui::Checkbox("Show Physics Colliders", &mRenderPhysics);
		ImGui::Separator();
		ImGuiColorPicker("Clear Color", anSceneManager::Get()->GetClearColor());
		ImGui::Separator();
	}

	ImGui::Checkbox("Open Lua File With Visual Studio Code", &mOpenLuaFileWithVSCode);
	
	ImGui::Separator();

	if (anProjectManager::IsProjectActive())
	{
		if (ImGui::Checkbox("Is Fullscreen", &anProjectManager::GetCurrentProject()->IsFullscreen))
			anProjectManager::SaveProject(anProjectManager::GetCurrentProject()->FullPath.string());

		ImGui::Separator();

		{
			const char* bodyTypeStrings[] = { "640x360", "854x480", "960x540", "1024x576", "1280x720", "1366x768", "1600x900", "1920x1080" };
			const char* currentBodyTypeString = bodyTypeStrings[anProjectManager::GetCurrentProject()->ResolutionID];
			if (ImGui::BeginCombo("Window Resolution", currentBodyTypeString))
			{
				for (int i = 0; i < DISPLAY_RESOLUTIONS_SIZE; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						anProjectManager::GetCurrentProject()->ResolutionID = i;
						anProjectManager::SaveProject(anProjectManager::GetCurrentProject()->FullPath.string());
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}
	}

	ImGui::End();

	ImGui::Begin("Renderer Stats");

	ImGui::Text("Frames Per Second: %d", mApplication->GetFramesPerSecond());
	ImGui::Text("Draw Calls: %d", anRenderer2D::Get().GetDrawCallCount());
	ImGui::Text("Vertex Count: %d", anRenderer2D::Get().GetVertexCount());
	ImGui::Text("Index Count: %d", anRenderer2D::Get().GetIndexCount());

	ImGui::End();

	ImGui::Begin("Script Editor");
	mTextEditorWindowFocused = ImGui::IsWindowFocused() || mTextEditor.IsTextBoxFocused();

	auto cpos = mTextEditor.GetCursorPosition();

	const anFileSystem::path& fileName = mTextEditorCurrentFilePath.filename();
	if (!fileName.empty())
	{
		mIsTextEditorFileSaved = mTextEditorFileSourceCode == GetExactTextEditorSource();
		ImGui::Text("Row: %d Column: %-d, %6d Lines  %s%s", cpos.mLine + 1, cpos.mColumn + 1, mTextEditor.GetTotalLines(),
			fileName.string().c_str(),
			!mIsTextEditorFileSaved ? "*" : " ");

		ImGui::SameLine();

		ImGui::Checkbox("Is Readonly", &mIsTextEditorFileReadonly);
		mTextEditor.SetReadOnly(mIsTextEditorFileReadonly);
	}

	mTextEditor.Render("ScriptEditor");
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			anFileSystem::path filePath(path);
			const anFileSystem::path extension = filePath.extension();
			if (extension == ".lua")
				LoadScriptToTextEditor(filePath);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();

	ImGui::Begin("Scene");
	mSceneWindowFocused = ImGui::IsWindowFocused();
	if (SceneIsValid())
	{
		DrawToolbar();
		OnScenePanel();
	}

	ImGui::End();

	ImGui::Begin("Entity");
	mEntityWindowFocused = ImGui::IsWindowFocused();

	if (SceneIsValid())
		OnEntityPanel();

	ImGui::End();

	ImGui::Begin("Console");
	if (ImGui::Button("Clear", { 60, 30 }))
		mLogs.clear();

	ImGui::Separator();

	for (const auto data : mLogs)
	{
		switch (data.Type)
		{
		case anLogType::Error: ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.0f, 0.0f, 1.0f}); break;
		case anLogType::Info: ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 1.0f, 0.0f, 1.0f }); break;
		case anLogType::Warning: ImGui::PushStyleColor(ImGuiCol_Text, { 0.9f, 0.9f, 0.0f, 1.0f }); break;
		}

		ImGui::Text("[%s] From: %s, %s", LogTypeToString(data.Type).c_str(), data.From.c_str(), data.Message.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::End();

	ImGui::Begin("Viewport");
	mViewportWindowFocused = ImGui::IsWindowFocused();
	mViewportWindowHovered = ImGui::IsWindowHovered();

	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	mViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	mViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	ImGui::Image(reinterpret_cast<void*>(mFramebuffer->GetTextureID()), { mViewportSize.x, mViewportSize.y }, { 0.0f, 1.0f }, { 1.0f, 0.0f });

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			anFileSystem::path filePath(path);
			const anFileSystem::path extension = filePath.extension();
			if (IsImageFile(extension.string()))
			{
				anTexture* texture = anLoadTexture(filePath.string());
				texture->SetEditorPath(filePath.lexically_relative(anProjectManager::GetCurrentProject()->Location).string());

				anEntity entity = anSceneManager::Get()->NewEntity(mNewEntityName);
				auto& transform = entity.GetComponent<anTransformComponent>();
				transform.Position = mExactViewportMousePosition;
				transform.Size.x = float(int(texture->GetWidth()));
				transform.Size.y = float(int(texture->GetHeight()));

				auto& sprite = entity.AddComponent<anSpriteRendererComponent>();
				sprite.Texture = texture;
			}
			else if (extension == ".anScene")
				OpenScene(filePath);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();

	ImGui::Begin("Asset Browser");
	mAssetBrowserWindowFocused = ImGui::IsWindowFocused();

	if (mAssetBrowserLocation != mProjectAssetsLocation)
	{
		if (ImGui::ImageButton((ImTextureID)mArrowIconTexture->GetID(), { 16, 16 }, { 0, 0 }, { 1, 1 }))
			mAssetBrowserLocation = mAssetBrowserLocation.parent_path();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				anFileSystem::path filePath(path);
				const anFileSystem::path newPath = mAssetBrowserLocation.parent_path() / filePath.filename();
				MoveFileItem(filePath, mAssetBrowserLocation.parent_path());
				OnAssetBrowserItemMove(filePath, newPath);
			}
			ImGui::EndDragDropTarget();
		}
	}

	ImGui::SameLine();

	ImGui::Text(mAssetBrowserLocation.string().c_str());
	// if (ImGui::BeginDragDropTarget())
	// {
	// 	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_ENTITY_ITEM"))
	// 	{
	// 		anEntity entity = *((anEntity*)payload->Data);
	// 		anGlobalSceneSerializer.SerializeEntity(mProjectLocation, entity, mAssetBrowserLocation / (entity.GetTag() + ".anEntity"));
	// 	}
	// 	ImGui::EndDragDropTarget();
	// }

	ImGui::Separator();

	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	for (const auto entry : anFileSystem::directory_iterator(mAssetBrowserLocation))
	{
		const anString fileName = entry.path().filename().string();

		ImGui::PushID(fileName.c_str());
		anTexture* icon = entry.is_directory() ? mFolderIconTexture : mFileIconTexture;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::ImageButton((ImTextureID)icon->GetID(), { thumbnailSize, thumbnailSize }, { 0, 0 }, { 1, 1 });
		if (entry.is_directory())
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					anFileSystem::path filePath(path);
					const anFileSystem::path newPath = entry.path() / filePath.filename();
					MoveFileItem(filePath, entry.path());
					OnAssetBrowserItemMove(filePath, newPath);
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (ImGui::BeginDragDropSource())
		{
			anFileSystem::path relativePath(entry.path());
			const wchar_t* itemPath = relativePath.c_str();
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (mLeftCtrl || mRightCtrl)
				{
					OnAssetBrowserItemRemove(entry.path());
					anFileSystem::remove_all(entry.path());
				}
				else
				{
					if (entry.is_directory())
						mAssetBrowserLocation /= entry.path().filename();

					if (entry.path().extension() == ".anScene")
						OpenScene(entry.path());
					else if(entry.path().extension() == ".lua")
					{
						if (mOpenLuaFileWithVSCode)
							OpenFileWithVSCode(entry.path());
						else
							LoadScriptToTextEditor(entry.path());
						// mEditorScene->ReloadScripts();
					}
				}
			}
		}
		
		ImGui::TextWrapped(fileName.c_str());

		ImGui::NextColumn();

		ImGui::PopID();
	}

	ImGui::Columns(1);

	if (ImGui::BeginPopupContextWindow(0, 1))
	{
		if (ImGui::MenuItem("New Scene"))
		{
			anString sceneName = "New Scene";
			if (anShowInputBox(sceneName, "New Scene", "Scene Name", sceneName))
			{
				const anFileSystem::path path = mAssetBrowserLocation / anFileSystem::path{ sceneName + ".anScene"};
				if (anFileSystem::exists(path))
					anShowMessageBox("Error", sceneName + " is existing scene", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);
				else
				{
					anScene* scene = new anScene();
					anGlobalSceneSerializer.SerializeScene(mProjectLocation, scene, path);
				}
			}
		}

		if (ImGui::MenuItem("New Lua Script"))
		{
			anString scriptName = "New Script";
			if (anShowInputBox(scriptName, "New Script", "Script Name", scriptName))
			{
				const anFileSystem::path path = mAssetBrowserLocation / anFileSystem::path{ scriptName + ".lua" };
				if (anFileSystem::exists(path))
					anShowMessageBox("Error", scriptName + " is existing lua script", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);
				else
				{
					anString noSpaceScriptName = scriptName;
					for (anUInt64 i = 0; i < noSpaceScriptName.size(); i++)
					{
						if (noSpaceScriptName[i] == ' ')
							noSpaceScriptName.erase(noSpaceScriptName.begin() + i);
					}

					anStringStream stream;
					stream << noSpaceScriptName << " = {}\n\n"
						"function " << noSpaceScriptName << ".setup()\n"
						"\tlocal obj = {}\n\n"
						"\treturn obj\n"
						"end\n\n"
						"function " << noSpaceScriptName << ".initialize(self)\n"
						"end\n\n"
						"function " << noSpaceScriptName << ".update(self, dt)\n"
						"end\n";

					anOutputFile file{ path };
					file << stream.str();
				}
			}
		}

		if (ImGui::MenuItem("New C++ Script"))
		{
			anString scriptName = "Cpp Script";
			if (anShowInputBox(scriptName, "Cpp Script", "Script Name", scriptName))
			{
				const anFileSystem::path path = mAssetBrowserLocation / anFileSystem::path{ scriptName + ".h" };
				if (anFileSystem::exists(path))
					anShowMessageBox("Error", scriptName + " is existing C++ script", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);
				else
					CreateNativeScript(mAssetBrowserLocation, scriptName);
			}
		}

		if (ImGui::MenuItem("New Folder"))
		{
			anString folderName = "New Folder";
			if (anShowInputBox(folderName, "New Folder", "Folder Name", folderName))
			{
				const anFileSystem::path path = mAssetBrowserLocation / folderName;
				if (anFileSystem::exists(path))
					anShowMessageBox("Error", folderName + " is existing folder", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);
				else
					anFileSystem::create_directory(path);
			}
		}

		if (ImGui::MenuItem("Open in Explorer"))
			anShellExecuteOpen(mAssetBrowserLocation.string());

		ImGui::EndPopup();
	}

	ImGui::End();

	EndImGuiDockspace();
}

#pragma region ImGui Dockspace

void anEditorState::BeginImGuiDockspace()
{
	// Note: Switch this to true to enable dockspace
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 250.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;
}

void anEditorState::EndImGuiDockspace()
{
	ImGui::End();
}

#pragma endregion

#pragma region Scene Panel

void anEditorState::OnScenePanel()
{
	if (anSceneManager::Get())
	{
		anSceneManager::Get()->GetRegistry().each([&](auto id)
			{
				anEntity entity{ id , anSceneManager::Get() };
				DrawEntityToScenePanel(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			mSelectedEntity = {};

		if (ImGui::BeginPopupContextWindow(0, 1))
		{
			if (ImGui::MenuItem("New Entity"))
				mSelectedEntity = anSceneManager::Get()->NewEntity(mNewEntityName);

			if (ImGui::MenuItem("New Camera"))
			{
				mSelectedEntity = anSceneManager::Get()->NewEntity("Camera");
				mSelectedEntity.AddComponent<anCameraComponent>();
			}

			if (ImGui::MenuItem("New Sprite Renderer"))
			{
				mSelectedEntity = anSceneManager::Get()->NewEntity("Sprite");
				mSelectedEntity.AddComponent<anSpriteRendererComponent>();
			}

			ImGui::EndPopup();
		}
	}
}

void anEditorState::DrawEntityToScenePanel(anEntity entity)
{
	auto& tag = entity.GetComponent<anTagComponent>().Tag;

	ImGuiTreeNodeFlags flags = ((mSelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity.GetHandle(), flags, tag.c_str());
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("SCENE_ENTITY_ITEM", &entity, sizeof(anEntity));
		ImGui::EndDragDropSource();
	}

	if (ImGui::IsItemClicked())
		mSelectedEntity = entity;

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Entity"))
			entityDeleted = true;

		ImGui::EndPopup();
	}

	if (opened)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
		if (opened)
			ImGui::TreePop();
		ImGui::TreePop();
	}

	if (entityDeleted)
	{
		anSceneManager::Get()->DestroyEntity(entity);
		if (mSelectedEntity == entity)
			mSelectedEntity = {};
	}
}

#pragma endregion

void anEditorState::OnEntityPanel()
{
	if (mSelectedEntity.GetHandle() != entt::null)
		DrawComponents(mSelectedEntity);
}

#pragma region Component Renderer Commands

template<typename T, typename UIFunction>
static void DrawComponent(const anString& name, anEntity entity, UIFunction uiFunction)
{
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
	if (entity.HasComponent<T>())
	{
		auto& component = entity.GetComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar(
		);
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("+"))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (T::GetComponentName() != "TransformComponent" && T::GetComponentName() != "UUIDComponent" && T::GetComponentName() != "TagComponent")
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;
			}

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component, entity);
			ImGui::TreePop();
		}

		if (removeComponent)
			entity.RemoveComponent<T>();
	}
}

static void DrawVec2Node(const std::string& label, anFloat2& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

static void DrawFloatNode(const anString& label, float& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

#pragma endregion

void anEditorState::DrawComponents(anEntity entity)
{
	if (entity.HasComponent<anTagComponent>())
	{
		auto& tag = entity.GetComponent<anTagComponent>().Tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		{
			tag = anString(buffer);
		}
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			anFileSystem::path filePath(path);
			const anString extension = filePath.extension().string();
			if (IsImageFile(extension))
			{
				if (!mSelectedEntity.HasComponent<anSpriteRendererComponent>())
				{
					auto& component = mSelectedEntity.AddComponent<anSpriteRendererComponent>();

					anTexture* texture = anLoadTexture(filePath.string());
					
					auto& transform = mSelectedEntity.GetComponent<anTransformComponent>();
					transform.Size.x = float(int(texture->GetWidth()));
					transform.Size.y = float(int(texture->GetHeight()));

					texture->SetEditorPath(filePath.lexically_relative(anProjectManager::GetCurrentProject()->Location).string());
					component.Texture = texture;
				}
			}

			if (extension == ".lua")
			{
				if (!mSelectedEntity.HasComponent<anLuaScriptComponent>())
				{
					auto& component = mSelectedEntity.AddComponent<anLuaScriptComponent>();

					component.Script = new anLuaScript();
					component.Script->LoadScript(filePath, filePath.lexically_relative(anProjectManager::GetCurrentProject()->Location));
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginPopup("AddComponent"))
	{
		DisplayAddComponentEntry<anCameraComponent>("Camera");
		DisplayAddComponentEntry<anSpriteRendererComponent>("Sprite Renderer");
		DisplayAddComponentEntry<anLuaScriptComponent>("Lua Script");
		DisplayAddComponentEntry<anRigidbodyComponent>("Rigidbody");
		DisplayAddComponentEntry<anBoxColliderComponent>("Box Collider");
		DisplayAddComponentEntry<anNativeScriptComponent>("Native Script");
		
		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	DrawComponent<anTransformComponent>("Transform", entity, [](auto& component, auto& entity)
		{
			DrawVec2Node("Translation", component.Position);
			DrawVec2Node("Scale", component.Size, 1.0f);
			DrawFloatNode("Rotation", component.Rotation);
		});

	DrawComponent<anCameraComponent>("Camera", entity, [](auto& component, auto& entity)
		{
			ImGui::Checkbox("Main Camera", &component.Current);
		});

	DrawComponent<anSpriteRendererComponent>("Sprite Renderer", entity, [](auto& component, auto& entity)	
		{
			ImGuiColorPicker("Color", component.Color);

			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					anFileSystem::path texturePath(path);
					const anFileSystem::path extension = texturePath.extension();
					if (IsImageFile(extension.string()))
					{
						anTexture* texture = anLoadTexture(texturePath.string());
						texture->SetEditorPath(texturePath.lexically_relative(anProjectManager::GetCurrentProject()->Location).string());
						component.Texture = texture;
						auto& size = entity.GetComponent<anTransformComponent>().Size;
						size.x = float(int(texture->GetWidth()));
						size.y = float(int(texture->GetHeight()));
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text(component.Texture == nullptr ? "No Texture" : component.Texture->GetEditorPath().c_str());
		
			ImGui::InputInt("Layer Number", &component.LayerNumber);
		});

	DrawComponent<anLuaScriptComponent>("Lua Script", entity, [](auto& component, auto& entity)
		{
			ImGui::Button("Script", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					anFileSystem::path filePath(path);
					if (filePath.extension() == ".lua")
					{
						component.Script = new anLuaScript();
						component.Script->LoadScript(filePath, filePath.lexically_relative(anProjectManager::GetCurrentProject()->Location));
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text(component.Script == nullptr ? "No Script" : component.Script->GetEditorPath().string().c_str());
		});

	DrawComponent<anRigidbodyComponent>("Rigidbody", entity, [](auto& component, auto& entity)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
		});

	DrawComponent<anBoxColliderComponent>("Box Collider", entity, [](auto& component, auto& entity)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
		});

	DrawComponent<anNativeScriptComponent>("Native Script", entity, [](auto& component, auto& entity)
		{
			ImGui::Button("Script", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					anFileSystem::path filePath(path);
					if (filePath.extension() == ".h")
					{
						component.Path = filePath.lexically_relative(anProjectManager::GetCurrentProject()->Location);
						anString stem = component.Path.stem().string();
						RemoveSpaces(stem);
						component.ClassName = stem;
					}
				}
				ImGui::EndDragDropTarget();
			}
	
			ImGui::SameLine();
			ImGui::Text(component.Path.empty() ? "No Script" : component.Path.string().c_str());
		});
}

template<typename T>
void anEditorState::DisplayAddComponentEntry(const anString& entryName) 
{
	if (!mSelectedEntity.HasComponent<T>())
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			mSelectedEntity.AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}
}

void anEditorState::DrawToolbar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	auto& colors = ImGui::GetStyle().Colors;
	const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
	const auto& buttonActive = colors[ImGuiCol_ButtonActive];
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

	bool toolbarEnabled = (bool)anSceneManager::Get();

	ImVec4 tintColor = ImVec4(1, 1, 1, 1);
	if (!toolbarEnabled)
		tintColor.w = 0.5f;

	const int iconSize = (int)mPlayButtonTexture->GetHeight();
	float size = (float)iconSize;
	
	if (ImGui::ImageButton((ImTextureID)(uint64_t)mPlayButtonTexture->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)) && toolbarEnabled)
		RunProject();
		
	ImGui::SameLine();

	const char* items[] = { "None", "Translate", "Scale", "Rotate" };
	const char* currentItem = items[mGizmoSystem.GetGizmoType() + 1];

	if (ImGui::BeginCombo("##gizmotype", currentItem, ImGuiComboFlags_NoArrowButton))
	{
		for (int i = 0; i < 4; i++)
		{
			bool isSelected = (currentItem == items[i]);
			if (ImGui::Selectable(items[i], isSelected))
				mGizmoSystem.SetGizmoType(i - 1);
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);
}

anEntity& anEditorState::GetSelectedEntity()
{
	return mSelectedEntity;
}

void anEditorState::SaveScene()
{
	if (!SceneIsValid())
		anShowMessageBox("Error", "You can't save the scene because there is no scene", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);
	else
		anSceneManager::Save(mProjectLocation.string(), mEditorScenePath.string());
}

void anEditorState::CloseScene()
{
	mSelectedEntity = {};
	anSceneManager::SetScene(nullptr);
	mNoScene = true;
	mApplication->GetWindow()->SetTitle("anEngine2D Editor - " + mProjectName + " - No Scene");
}

bool anEditorState::SceneIsValid() const
{
	return mNoScene != true && anSceneManager::Get() != nullptr;
}

void anEditorState::OpenScene(const anFileSystem::path& path)
{
	mEditorScenePath = path;
	anSceneManager::Load(mProjectLocation.string(), mEditorScenePath.string());
	mApplication->GetWindow()->SetTitle("anEngine2D Editor - " + mProjectName + " - " + path.filename().string());
	mNoScene = false;
}

bool anEditorState::IsImageFile(const anString& extension)
{
	return extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
		extension == ".bmp" || extension == ".hdr" || extension == ".psd" ||
		extension == ".tga" || extension == ".gif" || extension == ".pic" ||
		extension == ".psd";
}

void anEditorState::OnAssetBrowserItemMove(const anFileSystem::path& oldPath, const anFileSystem::path& newPath)
{
	const anFileSystem::path& oldEditorPath = oldPath.lexically_relative(mProjectLocation);
	const anFileSystem::path& newEditorPath = newPath.lexically_relative(mProjectLocation);
	if (IsImageFile(oldEditorPath.extension().string()))
	{
		auto view = anSceneManager::Get()->GetRegistry().view<anSpriteRendererComponent>();
		for(auto entity : view)
		{
			auto& component = view.get<anSpriteRendererComponent>(entity);
			if (!component.Texture)
				continue;

			if (anFileSystem::path{ component.Texture->GetEditorPath() } == anFileSystem::path{ oldEditorPath })
				component.Texture->SetEditorPath(newEditorPath.string());
		}
	}
	else if (oldEditorPath.extension() == ".lua")
	{
		auto view = anSceneManager::Get()->GetRegistry().view<anLuaScriptComponent>();
		for (auto entity : view)
		{
			auto& component = view.get<anLuaScriptComponent>(entity);
			if (!component.Script)
				continue;

			if (anFileSystem::path{ component.Script->GetEditorPath() } == anFileSystem::path{ oldEditorPath })
				component.Script->SetEditorPath(newEditorPath);
		}
	}
	else if (oldEditorPath.extension() == ".anScene")
	{
		if (mEditorScenePath == oldPath)
			mEditorScenePath = newPath;

		if (anFileSystem::path{ anProjectManager::GetCurrentProject()->StartScene } == anFileSystem::path{ oldEditorPath })
			SetStartingScene(newEditorPath);
	}
	else if (oldEditorPath.extension() == ".h" || oldEditorPath.extension() == ".cpp")
	{
		anFileSystem::path nPath = newEditorPath;
		anFileSystem::path oPath = oldEditorPath;
		if (oldEditorPath.extension() == ".h")
		{
			MoveFileItem(oldPath.parent_path() / (oldPath.stem().string() + ".cpp"), newPath.parent_path());
		}
		else if (oldEditorPath.extension() == ".cpp")
		{
			MoveFileItem(oldPath.parent_path() / (oldPath.stem().string() + ".h"), newPath.parent_path());
			nPath = nPath.parent_path() / (nPath.stem().string() + ".h");
			oPath = oPath.parent_path() / (oPath.stem().string() + ".h");
		}

		auto view = anSceneManager::Get()->GetRegistry().view<anNativeScriptComponent>();
		for (auto entity : view)
		{
			auto& component = view.get<anNativeScriptComponent>(entity);
			if (component.Path.empty())
				continue;

			if (anFileSystem::path{ component.Path } == anFileSystem::path{ oPath })
				component.Path = nPath;
		}

		GenerateNativeScriptProjectFile();
		GenerateNativeScriptProject();
	}

	SaveScene();
}

void anEditorState::MoveFileItem(const anFileSystem::path& fileName, const anFileSystem::path& newFolder)
{
	const anFileSystem::path newPath = newFolder / fileName.filename();
	if (anFileSystem::exists(newPath))
		anShowMessageBox("Error", newPath.filename().string() + " is exist item", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);
	else
		anFileSystem::rename(fileName, newPath);
}

void anEditorState::OnAssetBrowserItemRemove(const anFileSystem::path& fileName)
{
	const anFileSystem::path path = fileName.lexically_relative(mProjectLocation);
	if (IsImageFile(path.extension().string()))
	{
		auto view = anSceneManager::Get()->GetRegistry().view<anSpriteRendererComponent>();
		for (auto entity : view)
		{
			auto& component = view.get<anSpriteRendererComponent>(entity);
			if (!component.Texture)
				continue;
	
			if (anFileSystem::path{ component.Texture->GetEditorPath() } == path)
				component.Texture = nullptr;
		}
	}
	else if (path.extension() == ".lua")
	{
		if (mTextEditorCurrentFilePath == path)
			CloseTextEditorScript();

		auto view = anSceneManager::Get()->GetRegistry().view<anLuaScriptComponent>();
		for (auto entity : view)
		{
			auto& component = view.get<anLuaScriptComponent>(entity);
			if (!component.Script)
				continue;
	
			if (anFileSystem::path{ component.Script->GetEditorPath() } == path)
				component.Script = nullptr;
		}
	}
	else if (path.extension() == ".anScene")
	{
		if (mEditorScenePath == fileName)
			CloseScene();
	
		if (anFileSystem::path{ anProjectManager::GetCurrentProject()->StartScene } == path)
			SetStartingScene("");
	}
	else if (path.extension() == ".h" || path.extension() == ".cpp")
	{
		if (path.extension() == ".h")
		{
			anFileSystem::remove(path.parent_path() / (path.stem().string() + ".cpp"));
		}
		else if (path.extension() == ".cpp")
		{
			anFileSystem::remove(path.parent_path() / (path.stem().string() + ".h"));
		}

		auto view = anSceneManager::Get()->GetRegistry().view<anNativeScriptComponent>();
		for (auto entity : view)
		{
			auto& component = view.get<anNativeScriptComponent>(entity);
			if (component.Path.empty())
				continue;

			if (anFileSystem::path{ component.Path } == path)
				component.Path.clear();
		}

		GenerateNativeScriptProjectFile();
		GenerateNativeScriptProject();
	}

	SaveScene();
}

void anEditorState::SetStartingScene(const anFileSystem::path& path)
{
	mProjectStartScenePath = path;
	anProjectManager::GetCurrentProject()->StartScene = mProjectStartScenePath.string();
	anProjectManager::SaveProject(anProjectManager::GetCurrentProject()->FullPath.string());
}

void anEditorState::LoadScriptToTextEditor(const anFileSystem::path& path)
{
	mTextEditorCurrentFilePath = path;

	anInputFile file{ path };
	if (!file.good())
		anShowMessageBox("Error", path.string() + " couldn't opened", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);

	anStringStream stream;
	stream << file.rdbuf();

	mTextEditorFileSourceCode = stream.str();

	mTextEditor.SetText(mTextEditorFileSourceCode);
	mTextEditorFileSourceCode = GetExactTextEditorSource();

	mTextEditor.SetHandleKeyboardInputs(true);
	mTextEditor.SetHandleMouseInputs(true);
}

void anEditorState::SaveTextEditorFile()
{
	if (IsTextEditorHaveFile())
	{
		const anString src = GetExactTextEditorSource();
		
		mTextEditorFileSourceCode = src;
		
		anOutputFile file{ mTextEditorCurrentFilePath };
		file << src;
	}
}

bool anEditorState::IsTextEditorHaveFile()
{
	return mTextEditorCurrentFilePath.empty() == false;
}

void anEditorState::CloseTextEditorScript()
{
	if (IsTextEditorHaveFile())
	{
		mTextEditor.SetText("");
		mTextEditorCurrentFilePath.clear();

		mTextEditor.SetHandleKeyboardInputs(false);
		mTextEditor.SetHandleMouseInputs(false);
	}
}

void anEditorState::EditorInfo(const anString& msg)
{
	anLogData data;
	data.Type = anLogType::Info;
	data.From = "Editor";
	data.Message = msg;

	mLogs.push_back(data);
}

void anEditorState::EditorError(const anString& msg)
{
	anLogData data;
	data.Type = anLogType::Error;
	data.From = "Editor";
	data.Message = msg;

	mLogs.push_back(data);
}

void anEditorState::EditorWarning(const anString& msg)
{
	anLogData data;
	data.Type = anLogType::Warning;
	data.From = "Editor";
	data.Message = msg;

	mLogs.push_back(data);
}

void anEditorState::UserInfo(const anString& msg)
{
	anLogData data;
	data.Type = anLogType::Info;
	data.From = "User";
	data.Message = msg;

	mLogs.push_back(data);
}

void anEditorState::UserError(const anString& msg)
{
	anLogData data;
	data.Type = anLogType::Error;
	data.From = "User";
	data.Message = msg;

	mLogs.push_back(data);
}

void anEditorState::UserWarning(const anString& msg)
{
	anLogData data;
	data.Type = anLogType::Warning;
	data.From = "User";
	data.Message = msg;

	mLogs.push_back(data);
}

void anEditorState::LoadScene(const anString& path)
{
	mEditorScenePath = path;
	anSceneManager::Load(mProjectLocation.string(), path);
	mNoScene = false;
}

anString anEditorState::GetExactTextEditorSource()
{
	anString src = mTextEditor.GetText();
	if (src.back() == '\n')
		src.pop_back();

	return src;
}

void anEditorState::RenderOverlays()
{
	if (mRenderPhysics)
	{
		auto view = anSceneManager::Get()->GetRegistry().view<anTransformComponent, anBoxColliderComponent>();
		for (auto entity : view)
		{
			auto [tc, bc2d] = view.get<anTransformComponent, anBoxColliderComponent>(entity);

			anFloat2 translation = tc.Position + bc2d.Offset;
			anFloat2 scale = tc.Size * bc2d.Size * 2.0f;

			anMatrix4 transformation = glm::translate(glm::mat4(1.0f), { tc.Position, 0.0f })
				* glm::rotate(glm::mat4(1.0f), glm::radians(tc.Rotation), { 0.0f, 0.0f, 1.0f })
				* glm::translate(glm::mat4(1.0f), { bc2d.Offset, 0.0f })
				* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

			anRenderer2D::Get().DrawQuad(transformation, { 0, 255, 0 }, false);
		}
	}

	if (mSelectedEntity)
		anRenderer2D::Get().DrawQuad(mSelectedEntity.GetTransform().GetTransformationMatrix(), { 0, 0, 255 }, false);

	auto view = anSceneManager::Get()->GetRegistry().view<anTransformComponent, anCameraComponent>();
	for (auto entity : view)
	{
		auto [transform, camera] = view.get<anTransformComponent, anCameraComponent>(entity);

		int camW = (int)mCameraIconTexture->GetWidth();
		int camH = (int)mCameraIconTexture->GetHeight();
		anRenderer2D::Get().DrawTexture(mCameraIconTexture, transform.Position, { (float)camW, (float)camH }, { 255, 255, 255 }, false);
	}
}

void anEditorState::CloseProject()
{
	anProjectManager::SetCurrentProject(nullptr);
	mApplication->SetCurrentState<anProjectSelectorState>();
}

void anEditorState::DetectVSCode()
{
	mVSCodePath = anGetFolderPath(anFolderID::AppDataLocal);
	mVSCodePath /= "Programs\\Microsoft VS Code";
	mVSCodeDetected = anFileSystem::is_directory(mVSCodePath);
	if (mVSCodeDetected)
	{
		mVSCode = mVSCodePath / "bin\\code";
		if (!anFileSystem::exists(mVSCode))
			mVSCodeDetected = false;
	}

	if (!mVSCodeDetected)
	{
		anEditorWarning("The Visual Studio Code couldn't detected");
		anShowMessageBox("Warning", "The Visual Studio Code couldn't detected. You can download Visual Studio Code from 'https://code.visualstudio.com/download'.", anMessageBoxDialogType::Ok, anMessageBoxIconType::Warning);
		anShellExecuteOpen("https://code.visualstudio.com/download");
	}
}

void anEditorState::DetectVisualStudio()
{
	mVisualStudioPath = anGetFolderPath(anFolderID::ProgramFiles);
	mVisualStudioPath /= "Microsoft Visual Studio\\2022\\Community";
	mVisualStudioDetected = anFileSystem::is_directory(mVisualStudioPath);
	if (mVisualStudioDetected)
	{
		mVisualStudio = mVisualStudioPath / "Common7\\IDE\\devenv.exe";
		if (!anFileSystem::exists(mVisualStudio))
			mVisualStudioDetected = false;
	}

	if (!mVisualStudioDetected)
	{
		anEditorWarning("The Visual Studio couldn't detected");
		anShowMessageBox("Warning", "The Visual Studio couldn't detected. You can download Visual Studio from 'https://visualstudio.microsoft.com/downloads/'.", anMessageBoxDialogType::Ok, anMessageBoxIconType::Warning);
		anShellExecuteOpen("https://visualstudio.microsoft.com/downloads/");
	}
}

void anEditorState::ExecuteVSCodeCommand(const anString& cmd)
{
	if (mVSCodeDetected)
		anShellExecute("\"" + mVSCode.string() + "\"", cmd);
}

void anEditorState::OpenFileWithVSCode(const anFileSystem::path& path)
{
	anEditorInfo(path.string());
	ExecuteVSCodeCommand("-r \"" + path.string() + "\"");
}

void anEditorState::GenerateNativeScriptProjectPremake()
{
	GenerateNativeScriptProjectFile();
	anString incPath = mEditorPath.parent_path().string();
	anString incPath2 = anFileSystem::path{ mEditorPath.parent_path().parent_path() / "thirdparty" }.string();
	anString libPath = mEditorLibPath.string();
	anString rootPath = mEditorPath.parent_path().parent_path().string();
	anString binPath = mEditorBinPath.string();
	anString projectSource = mProjectSourceFile.string();
	ClearPath(incPath);
	ClearPath(incPath2);
	ClearPath(libPath);
	ClearPath(rootPath);
	ClearPath(binPath);
	ClearPath(projectSource);

	anOutputFile file(mProjectPremakeFile);
	file << "workspace \"" + mProjectName + "\"						 \n";
	file << "  location \"\"												 \n";
	file << "  architecture \"x86_64\"							 \n";
	file << "  startproject \"" + mProjectName + "\"				 \n";
	file << "																		 \n";
	file << "  configurations										 \n";
	file << "  {																	 \n";
	file << "	  \"Debug\",												 \n";
	file << "	  \"Release\"												 \n";
	file << "  }																	 \n";
	file << "																		 \n";
	file << "  defines														 \n";
	file << "  {																	 \n";
	file << "	   \"_CRT_SECURE_NO_WARNINGS\"				 \n";
	file << "  }																	 \n";
	file << "																		 \n";
	file << "  includedirs												\n";
	file << "  {																	 \n";
	file << "	   \"./\",														\n";
	file << "    \"" + incPath + "/anEngine2D\",               \n";
	file << "    \"" + incPath + "/anEngine2DEditor\",       \n";
	file << "    \"" + incPath2 + "/sol2/include\",  \n";
	file << "    \"" + incPath2 + "/entt/include\",  \n";
	file << "    \"" + incPath2 + "/glm\",					\n";
	file << "    \"" + incPath2 + "/stb\",					\n";
	file << "    \"" + incPath2 + "/tinyfiledialogs\",	\n";
	file << "    \"" + incPath2 + "/tinyxml2\",	\n";
	file << "    \"" + incPath2 + "/box2d/include\",	\n";
	file << "    \"" + incPath2 + "/fmod/include\",	\n";
	file << "    \"" + incPath2 + "/glew/include\",	\n";
	file << "    \"" + incPath2 + "/glfw/include\",	\n";
	file << "    \"" + incPath2 + "/freetype/include\",	\n";
	file << "    \"" + incPath2 + "/ImGui\",	\n";
	file << "    \"" + incPath2 + "/ImGuiColorTextEdit/ImGuiColorTextEdit\",	\n";
	file << "    \"" + incPath2 + "/lua\"	\n";
	file << "  }																	 \n";
	file << "																		 \n";
	file << "  filter \"system:windows\"						 \n";
	file << "	  defines													 \n";
	file << "	  {																 \n";
	file << "		  \"PLATFORM_WINDOWS\"						 \n";
	file << "	  }																 \n";
	file << "																		 \n";
	file << "  project \"" + mProjectName + "\"						 \n";
	file << "	  location \"\"											 \n";
	file << "	  kind \"WindowedApp\"								 \n";
	file << "	  language \"C++\"									 \n";
	file << "	  cppdialect \"C++17\"							 \n";
	file << "																		 \n";
	file << "	  targetdir \"./\"										 \n";
	file << "	  objdir \"obj\"		 \n";
	file << "																		 \n";
	file << "	  files														 \n";
	file << "	  {																 \n";
	file << "			\"" << projectSource << "\",\n";
	file << "		  \"assets/**.cpp\",			 \n";
	file << "		  \"assets/**.h\"				 \n";
	file << "	  }																 \n";
	file << "																		 \n";
	file << "		libdirs\n";
	file << "		{\n";
	file << "			\"" << libPath << "\"\n";
	file << "		}\n";
	file << "																		 \n";
	file << "	  links														 \n";
	file << "	  {																 \n";
	file << "			\"anEngine2D.lib\",               \n";
	file << "			\"anEngine2DEditor.lib\",          \n";
	file << "			\"Lua.lib\",               \n";
	file << "			\"ImGui.lib\",               \n";
	file << "			\"Box2D.lib\"               \n";
	file << "	  }																 \n";
	file << "																		 \n";
	file << "	  filter \"system:windows\"					 \n";
	file << "		  systemversion \"latest\"				 \n";
	file << "																		 \n";
	file << "			buildcommands {\n";
	file << "				\"del /S *.pdb\"\n";
	file << "			}\n";
	file << "																		 \n";
	file << "			postbuildcommands {\n";
	file << "				(\"{COPY} " << binPath << "/glfw3.dll ./\"),\n";
	file << "				(\"{COPY} " << binPath << "/glew32.dll ./\"),\n";
	file << "				(\"{COPY} " << binPath << "/freetype.dll ./\"),\n";
	file << "				(\"{COPY} " << binPath << "/fmod.dll ./\"),\n";
	file << "			}\n";
	file << "																		 \n";
	file << "	  filter \"configurations:Debug\"		 \n";
	file << "		  defines \"C_DEBUG\"							 \n";
	file << "		  runtime \"Debug\"								 \n";
	file << "		  symbols \"on\"									 \n";
	file << "																		 \n";
	file << "	  filter \"configurations:Release\"	 \n";
	file << "	  	defines \"C_RELEASE\"						 \n";
	file << "	  	runtime \"Release\"							 \n";
	file << "	  	optimize \"on\"									 \n\n";
}

void anEditorState::GenerateNativeScriptProject()
{
	if (!anFileSystem::exists(mProjectPremakeFile))
		GenerateNativeScriptProjectPremake();
	
	anShellExecute("premake5.exe", "--file=\"" + mProjectPremakeFile.string() + "\" vs2022");
}

void anEditorState::GenerateNativeScriptProjectFile()
{
	anOutputFile file(mProjectSourceFile);
	file << "//	Generated by anEngine2D																																									\n";
	file << "// Do not edit this file																																									\n";
	file << "#include \"Core/anApplication.h\"																							 \n";
	file << "#include \"Core/anEntryPoint.h\"																								 \n";
	file << "#include \"Core/anKeyCodes.h\"																									 \n";
	file << "#include \"State/anStateManager.h\"																						 \n";
	file << "#include \"Math/anMath.h\"																											 \n";
	file << "#include \"Project/anProject.h\"																									\n";
	file << "#include \"Device/anGPUCommands.h\"																							\n";
	file << "#include \"Core/anInputSystem.h\"																						\n";
	file << "#include \"Scene/anSceneSerializer.h\"																						\n";
	file << "#include \"Scene/anSceneManager.h\"																						\n";
	
	anVector<anString> scriptNames;
	IncludeAllHeadersFromAssets(file, scriptNames, mProjectAssetsLocation);

	file << "																																									\n";
	file << "class anGameState;																																									\n";
	file << "																																							 \n";
	file << "class anEngine2DApplication : public anApplication														 \n";
	file << "{																																						 \n";
	file << "public:																																			 \n";
	file << "	anEngine2DApplication()																											 \n";
	file << "		: anApplication({ \"" + mProjectName + "\", 1200, 700, false, false })		 \n";
	file << "	{																																						 \n";
	file << "	}																																						 \n";
	file << "																																							 \n";
	file << "	~anEngine2DApplication()																										 \n";
	file << "	{																																						 \n";
	file << "	}																																						 \n";
	file << "																																							 \n";
	file << "	void Initialize() override																									 \n";
	file << "	{																																						 \n";
	file << "		anInitializeRandomDevice();																								 \n";
	file << "																																							 \n";
	file << "		anProjectManager::LoadProject(\"" + mProjectName + ".anProj\");																																				\n";
	file << "																																							 \n";
	file << "		const int width = anDisplayResolution::DisplayResolutions[anProjectManager::GetCurrentProject()->ResolutionID].Width;																																																								\n";
	file << "		const int height = anDisplayResolution::DisplayResolutions[anProjectManager::GetCurrentProject()->ResolutionID].Height;																																																							\n";
	file << "																																							 \n";
	file << "		if (anProjectManager::GetCurrentProject()->IsFullscreen) mWindow->MakeFullscreen();																								 \n";
	file << "		mWindow->SetSize(width, height);																																																																		\n";
	file << "																																							 \n";
	file << "		SetCurrentState<anGameState>();																						 \n";
	file << "	}																																						 \n";
	file << "																																							 \n";
	file << "	void Update(float dt) override																							 \n";
	file << "	{																																						 \n";
	file << "	}																																						 \n";
	file << "																																							 \n";
	file << "	void OnEvent(const anEvent& event) override																	 \n";
	file << "	{																																						 \n";
	file << "	}																																						 \n";
	file << "																																							 \n";
	file << "	void OnImGuiRender() override																								 \n";
	file << "	{																																						 \n";
	file << "	}																																						 \n\n";
	file << "	void SetStartScenePath(const anString& path)\n";
	file << "	{																																						 \n";
	file << "		mStartScenePath = path;\n";
	file << "	}																																						 \n\n";
	file << "	anString GetStartScenePath() const\n";
	file << "	{																																						 \n";
	file << "		return mStartScenePath;\n";
	file << "	}																																						 \n\n";
	file << "private:\n";
	file << "	anString mStartScenePath;\n";
	file << "};																																						 \n";
	file << "																																							 \n";
	file << "class anGameState : public anState																								\n";
	file << "{																																								\n";
	file << "public:																																					\n";
	file << "	anGameState(anApplication* app)																									\n";
	file << "		: anState(app)																																\n";
	file << "	{																																								\n";
	file << "	}																																								\n";
	file << "																																									\n";
	file << "	~anGameState()																																	\n";
	file << "	{																																								\n";
	file << "	}																																								\n";
	file << "																																									\n";
	file << "	void Initialize() override																											\n";
	file << "	{																																								\n";
	file << "		mCurrentPath = anFileSystem::current_path();																																													\n";
	file << "																																																																					\n";
	file << "		mAssetsPath = mCurrentPath / \"assets\";																																															\n";
	file << "																																																																					\n";
	file << "		mApplication->GetWindow()->SetTitle(anProjectManager::GetCurrentProject()->Name);																											\n";
	file << "																																																																					\n";
	file << "		mStartScenePath = ((anEngine2DApplication*)mApplication)->GetStartScenePath();\n";
	file << "		if (mStartScenePath.empty()) mStartScenePath = anProjectManager::GetCurrentProject()->StartScene;\n";
	file << "																																																																					\n";
	file << "		mfWidth = (float)anDisplayResolution::DisplayResolutions[anProjectManager::GetCurrentProject()->ResolutionID].Width;																																																								\n";
	file << "		mfHeight = (float)anDisplayResolution::DisplayResolutions[anProjectManager::GetCurrentProject()->ResolutionID].Height;																																																							\n";
	file << "																																																																					\n";
	file << "		auto loadCallback = [&](anNativeScriptComponent& component, anEntity& entity)\n";
	file << "			{																								\n";

	for (const auto name : scriptNames)
	{
		file << "				if (component.ClassName == \"" << name << "\") {\n";
		file << "					component.Script = new " << name << "(entity);\n";
		file << "				}\n";
	}

	file << "			};																							\n\n";
	file << "		auto onEntityCopy = [&](anEntity& entity)\n";
	file << "			{																								\n";
	file << "				if (entity.HasComponent<anNativeScriptComponent>()) {\n";
	file << "					auto& comp = entity.GetComponent<anNativeScriptComponent>();\n\n";
	file << "					loadCallback(comp, entity);\n";
	file << "				}\n";
	file << "			};																							\n\n";
	file << "		anScene::SetOnEntityCopyCallback(onEntityCopy);\n";
	file << "		anGlobalSceneSerializer.SetLoadNativeScriptCallback(loadCallback);\n";
	file << "		anSceneManager::Load(mCurrentPath.string(), mStartScenePath);													\n";
	file << "																																																																					\n";
	file << "		anSceneManager::Get()->RuntimeInitialize();\n";
	file << "	}																																																																				\n";
	file << "																																																																					\n";
	file << "	void Update(float dt) override																																																					\n";
	file << "	{																																																																				\n";
	file << "		anSceneManager::Get()->OnViewportSize(int(mfWidth), int(mfHeight));																																									\n";
	file << "																																																																					\n";
	file << "		anClearColor(anSceneManager::Get()->GetClearColor());																																																\n";
	file << "		anEnableBlend();																																																											\n";
	file << "																																																																					\n";
	file << "		anSceneManager::Get()->RuntimeUpdate(dt);																																																						\n";
	file << "																																																																					\n";
	file << "		anRenderer2D::Get().End();																																																						\n";
	file << "	}																																																																				\n";
	file << "																																																																					\n";
	file << "	void OnEvent(const anEvent& event) override																																															\n";
	file << "	{																																																																				\n";
	file << "		if (event.Type == anEvent::WindowClose)																																																																				\n";
	file << "			anSceneManager::Get()->RuntimeStop();																																										\n\n";
	file << "		anSceneManager::Get()->RuntimeOnEvent(event);\n";
	file << "	}																																										\n";
	file << "																																											\n";
	file << "	void OnImGuiRender() override																												\n";
	file << "	{																																										\n";
	file << "	}																																										\n";
	file << "																																										\n";
	file << "private:																																									\n";
	file << "  anFileSystem::path mCurrentPath;\n";
	file << "  anFileSystem::path mAssetsPath;\n";
	file << "\n";
	file << "  float mfWidth = 0.0f;\n";
	file << "  float mfHeight = 0.0f;\n";
	file << "\n";
	file << "	 anString mStartScenePath;\n";
	file << "};																																										\n";
	file << "																																							 \n";
	file << "int anStartApplication(char** args, int argc)																 \n";
	file << "{																																						 \n";
	file << "	anEngine2DApplication* app = new anEngine2DApplication();										 \n";
	file << "	if (argc == 2) app->SetStartScenePath(args[1]);\n";
	file << "	app->Start();																																 \n";
	file << "																																							 \n";
	file << "	return 0;																																		 \n";
	file << "}																																						 \n";
}

void anEditorState::CreateNativeScript(const anFileSystem::path& loc, const anString& name)
{
	anString noSpace = name;
	RemoveSpaces(noSpace);
	const anString headerName = noSpace + ".h";
	const anString sourceName = noSpace + ".cpp";

	anOutputFile header(loc / headerName);
	header << "#ifndef " << name << "_h_\n";
	header << "#define " << name << "_h_\n\n";
	header << "#include \"Script/anNativeScript.h\"\n\n";
	header << "class " << name << " : public anNativeScript {\n";
	header << "public:\n";
	header << "\t" << name << "(anEntity owner);\n";
	header << "\t~" << name << "();\n\n";
	header << "\tvoid Initialize() override;\n";
	header << "\tvoid Update(float dt) override;\n";
	header << "\tvoid OnEvent(const anEvent& e) override;\n";
	header << "};\n\n";
	header << "#endif\n";

	anOutputFile source(loc / sourceName);
	source << "#include \"" << headerName << "\"\n\n";
	source << name << "::" << name << "(anEntity owner) : anNativeScript(owner)\n";
	source << "{\n";
	source << "}\n\n";
	source << name << "::~" << name << "()\n";
	source << "{\n";
	source << "}\n\n";
	source << "void " << name << "::Initialize()\n";
	source << "{\n";
	source << "}\n\n";
	source << "void " << name << "::Update(float dt)\n";
	source << "{\n";
	source << "}\n\n";
	source <<	"void " << name << "::OnEvent(const anEvent& e)\n";
	source << "{\n";
	source << "}\n\n";

	GenerateNativeScriptProjectFile();
	GenerateNativeScriptProject();
}

void anEditorState::IncludeAllHeadersFromAssets(anOutputFile& file, anVector<anString>& names, const anFileSystem::path& path)
{
	for (const auto entry : anFileSystem::directory_iterator(path))
	{
		if (entry.path().extension() == ".h")
		{
			anString loc = entry.path().lexically_relative(mProjectLocation).string();
			ClearPath(loc);
			file << "#include \"" << loc << "\"\n";

			anString stem = entry.path().stem().string();
			RemoveSpaces(stem);
			names.push_back(stem);
		}

		if (entry.is_directory())
			IncludeAllHeadersFromAssets(file, names, entry.path());
	}
}

void anEditorState::ClearPath(anString& path, char r)
{
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '\\' || path[i] == '/')
			path[i] = r;
	}
}

void anEditorState::RemoveSpaces(anString& src)
{
	for (int i = 0; i < src.size(); i++)
	{
		if (src[i] == ' ')
			src.erase(src.begin() + i);
	}
}

void anEditorState::ExecuteVisualStudioCommand(const anString& cmd)
{
	anShellExecute("\"" + mVisualStudio.string() + "\"", cmd, true);
}

void anEditorState::OpenSolutionWithVisualStudio(const anFileSystem::path& path)
{
	if (!anFileSystem::exists(mProjectSolution))
		GenerateNativeScriptProject();

	ExecuteVisualStudioCommand("\"" + path.string() + "\"");
}

void anEditorState::BuildSolution()
{
	anFileSystem::remove(mProjectLocation / (mProjectName + ".exe"));

	if (!anFileSystem::exists(mProjectSolution))
		GenerateNativeScriptProject();

	if (!anFileSystem::exists(mProjectBuildFile))
		GenerateBuildFile();

	anShellExecuteOpen(mProjectBuildFile.string(), false);
}

void anEditorState::GenerateBuildFile()
{
	anString vsPath = mVisualStudio.string();
	anString solPath = mProjectSolution.string();
	ClearPath(vsPath);
	ClearPath(solPath);

	anOutputFile buildFile(mProjectBuildFile);
	buildFile << "call \"" << vsPath << "\" " << "\"" << solPath << "\" /Build";
}

static void _CopyItem(const anFileSystem::path& first, const anFileSystem::path& second)
{
	anFileSystem::path loc = second / first.filename();
	anFileSystem::copy(first, loc);
}

void anEditorState::ExportProject()
{
	anString in;
	if (anSelectFolderDialog(in, "Export Project"))
	{
		anString loc = (anFileSystem::path{ in } / mProjectName).string();
		ClearPath(loc);

		const anFileSystem::path exeFile = mProjectLocation / (mProjectName + ".exe");
		const anFileSystem::path glfwFile = mProjectLocation / "glfw3.dll";
		const anFileSystem::path glewFile = mProjectLocation / "glew32.dll";
		const anFileSystem::path freetypeFile = mProjectLocation / "glew32.dll";
		const anFileSystem::path fmodFile = mProjectLocation / "fmod.dll";
		if (!anFileSystem::exists(exeFile))
			anShowMessageBox("Error", "Couldn't find '" + mProjectName + ".exe' please build the project.", anMessageBoxDialogType::Ok, anMessageBoxIconType::Error);
		if (!anFileSystem::exists(glfwFile))
			anShowMessageBox("Error", "Couldn't find 'glfw3.dll' please build the project.", anMessageBoxDialogType::Ok, anMessageBoxIconType::Error);
		if (!anFileSystem::exists(glewFile))
			anShowMessageBox("Error", "Couldn't find 'glew32.dll' please build the project.", anMessageBoxDialogType::Ok, anMessageBoxIconType::Error);
		if (!anFileSystem::exists(fmodFile))
			anShowMessageBox("Error", "Couldn't find 'fmod.dll' please build the project.", anMessageBoxDialogType::Ok, anMessageBoxIconType::Error);
		if (!anFileSystem::exists(freetypeFile))
			anShowMessageBox("Error", "Couldn't find 'freetype.dll' please build the project.", anMessageBoxDialogType::Ok, anMessageBoxIconType::Error);

		anFileSystem::create_directory(loc);
		_CopyItem(mProjectLocation / (mProjectName + ".anProj"), loc);
		_CopyItem(mProjectAssetsLocation, loc);
		_CopyItem(exeFile, loc);
		_CopyItem(glfwFile, loc);
		_CopyItem(glewFile, loc);
		_CopyItem(freetypeFile, loc);
		_CopyItem(fmodFile, loc);

		anShellExecuteOpen(loc);
	}
}

void anEditorState::RunProject()
{
	if (!mRunProject)
	{
		BuildSolution();
		mRunProject = true;
	}
}
