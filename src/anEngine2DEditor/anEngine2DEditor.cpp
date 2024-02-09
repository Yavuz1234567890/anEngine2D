#include "anEngine2DEditor.h"

#include <imgui/imgui.h>

anEngine2DApplication::anEngine2DApplication()
	: anApplication({ "anEngine2D Editor", 1200, 700, true, true })
{
}

anEngine2DApplication::~anEngine2DApplication()
{
}

void anEngine2DApplication::PrepareImGuiStyle()
{
	ImGuiIO io = ImGui::GetIO();

	float fontSize = 20.0f;
	io.Fonts->AddFontFromFileTTF("fonts/raleway/Raleway-Bold.ttf", fontSize);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("fonts/raleway/Raleway-Regular.ttf", fontSize);

	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.859f, 0.482f, 0.129f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.62f, 0.357f, 0.11f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.63f, 0.63f, 0.63f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.859f, 0.482f, 0.129f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.62f, 0.357f, 0.11f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.63f, 0.63f, 0.63f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.859f, 0.482f, 0.129f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.62f, 0.357f, 0.11f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.63f, 0.63f, 0.63f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.859f, 0.482f, 0.129f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.62f, 0.357f, 0.11f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.63f, 0.63f, 0.63f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Resizing Grip
	colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
	colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
	colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
}

void anEngine2DApplication::Initialize()
{
	PrepareImGuiStyle();

	anInitializeRandomDevice();

	anFileSystem::create_directory("projects");

	SetCurrentState<anProjectSelectorState>();
}

void anEngine2DApplication::Update(float dt)
{
}

void anEngine2DApplication::OnEvent(const anEvent& event)
{
}

void anEngine2DApplication::OnImGuiRender()
{
}
