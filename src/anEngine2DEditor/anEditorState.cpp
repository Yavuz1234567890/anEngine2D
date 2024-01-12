#include "anEditorState.h"
#include "Device/anGPUCommands.h"
#include "Core/anKeyCodes.h"
#include "Math/anMath.h"
#include "Project/anProject.h"
#include "Core/anMessage.h"
#include "Core/anInputSystem.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

anEditorState::anEditorState(anApplication* app)
	: anState(app)
{
}

anEditorState::~anEditorState()
{
}

void anEditorState::Initialize()
{
	mPlayButtonTexture = anLoadTexture("icons/playbutton.png");
	mStopButtonTexture = anLoadTexture("icons/stopbutton.png");
	mCameraIconTexture = anLoadTexture("icons/cameraicon.png");
	mFolderIconTexture = anLoadTexture("icons/directoryicon.png");
	mFileIconTexture = anLoadTexture("icons/fileicon.png");

	if (!anProjectManager::GetCurrentProject())
		mApplication->GetWindow()->Close();

	mProjectLocation = anProjectManager::GetCurrentProject()->Location;
	mProjectName = anProjectManager::GetCurrentProject()->Name;

	mProjectAssetsLocation = mProjectLocation / "assets";
											   
	mAssetBrowserLocation = mProjectAssetsLocation;
											   
	mProjectStartScenePath = mProjectLocation / anProjectManager::GetCurrentProject()->StartScene;

	if (!anProjectManager::GetCurrentProject()->StartScene.empty() && anFileSystem::exists(mProjectStartScenePath))
	{
		mEditorScenePath = mProjectStartScenePath;
		mEditorScene = mSceneSerializer.DeserializeScene(mProjectLocation, mProjectStartScenePath);
		mNoScene = false;
	}

	mApplication->GetWindow()->SetTitle("anEngine2D Editor - " + mProjectName + " - " + (mNoScene ? "No Scene" : anFileSystem::path{mEditorScenePath}.filename().string()));

	anFileSystem::create_directory(mProjectAssetsLocation);
	
	mFramebuffer = new anFramebuffer({ mApplication->GetWindow()->GetStartWidth(), mApplication->GetWindow()->GetStartHeight() });

	{
		const int iWidth = (int)mApplication->GetWindow()->GetStartWidth();
		const int iHeight = (int)mApplication->GetWindow()->GetStartHeight();

		mfWidth = (float)iWidth;
		mfHeight = (float)iHeight;
	}

	mGizmoSystem.Initialize(this);
}

void anEditorState::Update(float dt)
{
	if (SceneIsValid())
		mEditorScene->OnViewportSize(anUInt32(mViewportSize.x), anUInt32(mViewportSize.y));

	anFramebufferSpecification spec = mFramebuffer->GetSpecification();
	if (mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && (spec.Width != mViewportSize.x || spec.Height != mViewportSize.y))
	{
		mFramebuffer->Resize((anUInt32)mViewportSize.x, (uint32_t)mViewportSize.y);
		mEditorCamera.SetOrtho(mViewportSize.x * -0.5f, mViewportSize.x * 0.5f, mViewportSize.y * -0.5f, mViewportSize.y * 0.5f);
	}

	anClear();
	anEnableBlend();

	mFramebuffer->Bind();

	anClear();
	anEnableBlend();

	auto [mx, my] = ImGui::GetMousePos();
	mx -= mViewportBounds[0].x;
	my -= mViewportBounds[0].y;
	anFloat2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
	my = viewportSize.y - my;
	mMousePosition = { (float)mx, (float)my };
	mLastExactViewportMousePosition = mExactViewportMousePosition;
	mExactViewportMousePosition = ((mMousePosition - (mViewportBounds[1] - mViewportBounds[0]) * 0.5f) * anFloat2(1.0f, -1.0f)) + mEditorCamera.GetPosition();

	if (SceneIsValid())
	{
		if (mSceneState == anSceneState::Edit)
		{
			mEditorScene->EditorUpdate(dt, mEditorCamera, mCameraIconTexture);

			mGizmoSystem.UpdateGizmos(dt, mExactViewportMousePosition);
			anRenderer2D::Get().End();

			if (mMousePosition.x >= 0 && mMousePosition.y >= 0 && mMousePosition.x < viewportSize.x && mMousePosition.y < viewportSize.y && mDragEditorCamera && !mGizmoSystem.IsUsing())
			{
				mEditorCamera.Move((mLastMousePosition - mMousePosition) * mEditorCameraSpeed * anFloat2(1.0f, -1.0f));
			}
		}

		if (mSceneState == anSceneState::Runtime)
		{
			if (mEditorScene->RuntimeUpdate(dt))
			{

				anRenderer2D::Get().End();
			}
		}
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

	mCtrl = anInputSystem::IsKey(anKeyLeftControl) || anInputSystem::IsKey(anKeyRightControl);

	if (SceneIsValid())
	{
		mGizmoSystem.OnEvent(event);

		if (event.Type == anEvent::KeyDown)
		{
			if (event.KeyCode == anKeyS)
			{
				if (mCtrl)
					SaveScene();
			}

			if (event.KeyCode == anKeyW)
			{
				if (mCtrl)
					CloseScene();
			}

			if (event.KeyCode == anKeyR)
			{
				if (mCtrl)
					mEditorScene->ReloadScripts();
			}

			if (event.KeyCode == anKeyDelete)
			{
				if (GImGui->ActiveId == 0)
				{
					anEntity ent = mSelectedEntity;
					if (ent.GetHandle() != entt::null)
					{
						mEditorScene->DestroyEntity(ent);
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

			if (ImGui::MenuItem("Set Starting Scene"))
			{
				if (mNoScene)
					anShowMessageBox("Error", "The current scene is fresh scene", anMessageBoxDialogType::OkCancel, anMessageBoxIconType::Error);
				else
				{
					mProjectStartScenePath = mEditorScenePath.lexically_relative(mProjectLocation);
					anProjectManager::GetCurrentProject()->StartScene = mProjectStartScenePath.string();
					anProjectManager::SaveProject(anProjectManager::GetCurrentProject()->FullPath.string());
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				SaveScene();

			if (ImGui::MenuItem("Close Scene", "Ctrl+W"))
				CloseScene();

			if (ImGui::MenuItem("Reload Scripts", "Ctrl+R"))
				mEditorScene->ReloadScripts();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Begin("Scene");
	if (SceneIsValid())
	{
		DrawToolbar();
		OnScenePanel();
	}

	ImGui::End();

	ImGui::Begin("Entity");
	if (SceneIsValid())
		OnEntityPanel();

	ImGui::End();

	ImGui::Begin("Viewport");

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
				texture->SetScenePath(filePath.lexically_relative(anProjectManager::GetCurrentProject()->Location).string());

				anEntity entity = mEditorScene->NewEntity(mNewEntityName);
				auto& transform = entity.GetComponent<anTransformComponent>();
				transform.Position = mExactViewportMousePosition;
				transform.Size.x = float(int(texture->GetWidth()));
				transform.Size.y = float(int(texture->GetHeight()));

				auto& sprite = entity.AddComponent<anSpriteRendererComponent>();
				sprite.Texture = texture;
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();

	ImGui::Begin("Asset Browser");

	if (mAssetBrowserLocation != mProjectAssetsLocation)
	{
		if (ImGui::Button("<-"))
			mAssetBrowserLocation = mAssetBrowserLocation.parent_path();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				anFileSystem::path filePath(path);
				anFileSystem::rename(filePath, mAssetBrowserLocation.parent_path() / filePath.filename());
			}
			ImGui::EndDragDropTarget();
		}
	}

	ImGui::SameLine();

	ImGui::Text(mAssetBrowserLocation.string().c_str());

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
					anFileSystem::rename(filePath, entry.path() / filePath.filename());
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
				if (mCtrl)
					anFileSystem::remove_all(entry.path());
				else
				{
					if (entry.is_directory())
						mAssetBrowserLocation /= entry.path().filename();

					if (entry.path().extension() == ".anScene")
						OpenScene(entry.path());
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
					mSceneSerializer.SerializeScene(mProjectLocation, scene, path);
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
	if (mEditorScene)
	{
		mEditorScene->GetRegistry().each([&](auto id)
			{
				anEntity entity{ id , mEditorScene };
				DrawEntityToScenePanel(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			mSelectedEntity = {};

		if (ImGui::BeginPopupContextWindow(0, 1))
		{
			if (ImGui::MenuItem("New Entity"))
				mSelectedEntity = mEditorScene->NewEntity(mNewEntityName);

			if (ImGui::MenuItem("New Camera"))
			{
				mSelectedEntity = mEditorScene->NewEntity("Camera");
				mSelectedEntity.AddComponent<anCameraComponent>();
			}

			if (ImGui::MenuItem("New Sprite Renderer"))
			{
				mSelectedEntity = mEditorScene->NewEntity("Sprite");
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
		mEditorScene->DestroyEntity(entity);
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
			if (ImGui::MenuItem("Remove Component"))
				removeComponent = true;

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

					texture->SetScenePath(filePath.lexically_relative(anProjectManager::GetCurrentProject()->Location).string());
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
			float colors[4];
			colors[0] = (float)component.Color.R / 255.0f;
			colors[1] = (float)component.Color.G / 255.0f;
			colors[2] = (float)component.Color.B / 255.0f;
			colors[3] = (float)component.Color.A / 255.0f;

			ImGui::ColorEdit4("Color", colors);

			component.Color.R = colors[0] * 255;
			component.Color.G = colors[1] * 255;
			component.Color.B = colors[2] * 255;
			component.Color.A = colors[3] * 255;

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
						texture->SetScenePath(texturePath.lexically_relative(anProjectManager::GetCurrentProject()->Location).string());
						component.Texture = texture;
						auto& size = entity.GetComponent<anTransformComponent>().Size;
						size.x = float(int(texture->GetWidth()));
						size.y = float(int(texture->GetHeight()));
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text(component.Texture == nullptr ? "No Texture" : component.Texture->GetScenePath().c_str());
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

	bool toolbarEnabled = (bool)mEditorScene;

	ImVec4 tintColor = ImVec4(1, 1, 1, 1);
	if (!toolbarEnabled)
		tintColor.w = 0.5f;

	const int iconSize = (int)mPlayButtonTexture->GetHeight();
	float size = (float)iconSize;
	
	anTexture* icon = mSceneState == anSceneState::Edit ? mPlayButtonTexture : mStopButtonTexture;

	if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)) && toolbarEnabled)
	{
		if (mSceneState == anSceneState::Edit)
		{
			mRuntimeScene = mEditorScene;
			mEditorScene->RuntimeInitialize();
			mSceneState = anSceneState::Runtime;
		}
		else if (mSceneState == anSceneState::Runtime)
		{
			mEditorScene = mRuntimeScene;
			mSceneState = anSceneState::Edit;
		}
	}

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
		mSceneSerializer.SerializeScene(mProjectLocation, mEditorScene, mEditorScenePath);
}

void anEditorState::CloseScene()
{
	mSelectedEntity = {};
	mEditorScene = nullptr;
	mNoScene = true;
	mApplication->GetWindow()->SetTitle("anEngine2D Editor - " + mProjectName + " - No Scene");
}

bool anEditorState::SceneIsValid() const
{
	return mNoScene != true && mEditorScene != nullptr;
}

void anEditorState::OpenScene(const anFileSystem::path& path)
{
	mEditorScenePath = path;
	mEditorScene = mSceneSerializer.DeserializeScene(mProjectLocation, mEditorScenePath);
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
