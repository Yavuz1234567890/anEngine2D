#include "anProjectSelectorState.h"
#include "Core/anFileSystem.h"
#include "Project/anProject.h"
#include "anEditorState.h"

#include <imgui/imgui.h>

anProjectSelectorState::anProjectSelectorState(anApplication* app)
	: anState(app)
{
}

anProjectSelectorState::~anProjectSelectorState()
{
}

void anProjectSelectorState::Initialize()
{
	mApplication->GetWindow()->SetTitle("anEngine2D Editor - Project Selector");
	mProjectFolder = anFileSystem::current_path().string() + "\\projects";
}

void anProjectSelectorState::Update(float dt)
{
}

void anProjectSelectorState::OnEvent(const anEvent& event)
{
}

void anProjectSelectorState::OnImGuiRender()
{
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
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

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGui::SetCursorPosY(60);
	ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(anString("Project Folder: " + mProjectFolder).c_str()).x - 150.0f) * 0.5f);
	ImGui::Text("Project Folder: %s", mProjectFolder.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Browse", { 150, 30 }))
		anSelectFolderDialog(mProjectFolder, "Select Project Folder", mProjectFolder);

	ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x - ImGui::CalcTextSize("Create Project Folder").x + 30.0f) * 0.5f);
	ImGui::Checkbox("Create Project Folder", &mCreateProjectFolder);
	
	ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x - 75) * 0.5f);
	if (ImGui::Button("New Project", { 150, 30 }))
	{
		anString name = "anEngine2D Project";
		if (anShowInputBox(name, "New Project", "Project Name", name))
		{
			const anString projectFolder = mCreateProjectFolder ? (mProjectFolder + "\\" + name + "\\") : (mProjectFolder + "\\");
			if (mCreateProjectFolder)
			{
				if (anFileSystem::create_directory(projectFolder))
				{
					anProjectManager::SetCurrentProject(new anProject());
					anProjectManager::GetCurrentProject()->FullPath = projectFolder + name + ".anProj";
					anProjectManager::GetCurrentProject()->Location = projectFolder;
					anProjectManager::GetCurrentProject()->Name = name;
					anProjectManager::SaveProject(anProjectManager::GetCurrentProject()->FullPath);
					mApplication->SetCurrentState<anEditorState>();
				}
				else
					anShowMessageBox("Error", name + " project existing", anMessageBoxDialogType::Ok, anMessageBoxIconType::Error);
			}
			else
			{
				anProjectManager::SetCurrentProject(new anProject());
				anProjectManager::GetCurrentProject()->FullPath = projectFolder + name + ".anProj";
				anProjectManager::GetCurrentProject()->Location = projectFolder;
				anProjectManager::GetCurrentProject()->Name = name;
				anProjectManager::SaveProject(anProjectManager::GetCurrentProject()->FullPath);
				mApplication->SetCurrentState<anEditorState>();
			}
		}
	}

	ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x - 75) * 0.5f);
	if (ImGui::Button("Open Project", { 150, 30 }))
	{
		anString file;
		if (anOpenFileDialog(file, "Open Project", mProjectExtension, "anEngine2D Project File"))
		{
			anProjectManager::LoadProject(file);
			anProjectManager::GetCurrentProject()->Location = anFileSystem::path{ file }.parent_path().string();
			anProjectManager::GetCurrentProject()->FullPath = file;
			mApplication->SetCurrentState<anEditorState>();
		}
	}

	ImGui::End();
}
