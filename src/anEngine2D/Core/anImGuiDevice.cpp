#include "anImGuiDevice.h"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
