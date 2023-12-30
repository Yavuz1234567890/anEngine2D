#include "anImGuiDevice.h"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

anImGuiDevice::anImGuiDevice()
{
}

anImGuiDevice::~anImGuiDevice()
{
}

void anImGuiDevice::Initialize(anWindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   

	ImGui::StyleColorsDark();

	GLFWwindow* wnd = (GLFWwindow*)window->GetHandle();

	ImGui_ImplGlfw_InitForOpenGL(wnd, true);
	ImGui_ImplOpenGL3_Init("#version 410");
}

void anImGuiDevice::PrepareCustomStyle()
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

void anImGuiDevice::Start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void anImGuiDevice::End()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
