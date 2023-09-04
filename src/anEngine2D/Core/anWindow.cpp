#include "anWindow.h"
#include "anMessage.h"
#include "Device/anGPUCommands.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static bool sGLFWInitialized = false;
static bool sGLEWInitialized = false;

class anGLFWWindow : public anWindow
{
public:
	anGLFWWindow(const anString& title, anUInt32 width, anUInt32 height)
		: anWindow(title, width, height)
	{
		if (!sGLFWInitialized)
		{
			if (!glfwInit())
				anShowMessageBox("GLFW initialization failed!");

			sGLFWInitialized = true;
		}

		mHandle = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		glfwMakeContextCurrent(mHandle);
		glfwSwapInterval(mIsVSync ? 1 : 0);

		if (!sGLEWInitialized)
		{
			glewExperimental = GL_TRUE;
			if (glewInit())
				anShowMessageBox("OpenGL initialization failed!");

			sGLEWInitialized = true;
		}

		anSetViewport({ 0.0f, 0.0f }, { (float)width, (float)height });
	}

	~anGLFWWindow()
	{
		if (mHandle)
		{
			glfwDestroyWindow(mHandle);
			mHandle = NULL;
		}
	}

	void Present() override
	{
		glfwPollEvents();
		glfwSwapBuffers(mHandle);
	}

	void Close() override
	{
		glfwSetWindowShouldClose(mHandle, GLFW_TRUE);
	}

	bool IsRunning() const override
	{
		return glfwWindowShouldClose(mHandle) == 0;
	}

	void MakeFullscreen() override
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(mHandle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

		glViewport(0, 0, mode->width, mode->height);
	}

	void SetTitle(const anString& title) override
	{
		glfwSetWindowTitle(mHandle, title.c_str());
	}

	void SetVSync(bool vsync) override
	{
		if (glfwGetCurrentContext() != mHandle)
			return;

		if (mIsVSync != vsync)
		{
			glfwSwapInterval(vsync ? 1 : 0);
			mIsVSync = vsync;
		}
	}

private:
	GLFWwindow* mHandle = nullptr;
};

anWindow* anCreateWindow(const anString& title, anUInt32 width, anUInt32 height)
{
	return new anGLFWWindow(title, width, height);
}
