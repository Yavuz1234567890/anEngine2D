#include "anWindow.h"
#include "anMessage.h"
#include "Device/anGPUCommands.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

static bool sGLFWInitialized = false;
static bool sGLEWInitialized = false;

class anGLFWWindow : public anWindow
{
public:
	anGLFWWindow(const anString& title, anUInt32 width, anUInt32 height, const anEventCallback& callback, bool isResizable = false)
		: anWindow(title, width, height, callback, isResizable)
	{
		if (!sGLFWInitialized)
		{
			if (!glfwInit())
				anShowMessageBox("GLFW initialization failed!");

			sGLFWInitialized = true;
		}

		if (!isResizable)
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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

		glfwSetWindowUserPointer(mHandle, (void*)this);

		glfwSetKeyCallback(mHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				anGLFWWindow* wnd = (anGLFWWindow*)glfwGetWindowUserPointer(window);
				
				if (action == GLFW_PRESS)
					wnd->OnKeyDown((anUInt32)key);
				if (action == GLFW_RELEASE)
					wnd->OnKeyUp((anUInt32)key);
			});

		glfwSetMouseButtonCallback(mHandle, [](GLFWwindow* window, int button, int action, int mods)
			{
				anGLFWWindow* wnd = (anGLFWWindow*)glfwGetWindowUserPointer(window);

				double mx = 0.0;
				double my = 0.0;
				glfwGetCursorPos(window, &mx, &my);
				const anFloat2 mousePos = { (float)mx, (float)my };

				if (action == GLFW_PRESS)
					wnd->OnMouseButtonDown((anUInt32)button, mousePos);
				if (action == GLFW_RELEASE)
					wnd->OnMouseButtonUp((anUInt32)button, mousePos);
			});

		glfwSetCursorPosCallback(mHandle, [](GLFWwindow* window, double mx, double my)
			{
				anGLFWWindow* wnd = (anGLFWWindow*)glfwGetWindowUserPointer(window);

				const anFloat2 mousePos = { (float)mx, (float)my };

				wnd->OnMouseMove(mousePos);
			});

		glfwSetScrollCallback(mHandle, [](GLFWwindow* window, double x, double y)
			{
				anGLFWWindow* wnd = (anGLFWWindow*)glfwGetWindowUserPointer(window);

				const anFloat2 axis = { (float)x, (float)y };
				
				wnd->OnMouseWheel(axis);
			});

		glfwSetWindowSizeCallback(mHandle, [](GLFWwindow* window, int width, int height)
			{
				anGLFWWindow* wnd = (anGLFWWindow*)glfwGetWindowUserPointer(window);

				wnd->OnWindowSize(width, height);
			});

		glfwSetWindowPosCallback(mHandle, [](GLFWwindow* window, int x, int y)
			{
				anGLFWWindow* wnd = (anGLFWWindow*)glfwGetWindowUserPointer(window);

				wnd->OnWindowMove(x, y);
			});

		glfwSetWindowCloseCallback(mHandle, [](GLFWwindow* window)
			{
				anGLFWWindow* wnd = (anGLFWWindow*)glfwGetWindowUserPointer(window);

				wnd->OnWindowClose();
			});
	}

	~anGLFWWindow()
	{
		if (mHandle)
		{
			glfwDestroyWindow(mHandle);
			mHandle = NULL;
		}
	}

	void SetWindowIcon(const void* data, int width, int height)
	{
		GLFWimage images[1];
		images[0].pixels = (unsigned char*)data;
		images[0].width = (int)width;
		images[0].height = (int)height;
		glfwSetWindowIcon(mHandle, 1, images);
	}

	void Present() override
	{
		glfwPollEvents();
		glfwSwapBuffers(mHandle);
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

anWindow* anCreateWindow(const anString& title, anUInt32 width, anUInt32 height, const anEventCallback& callback, bool resizable)
{
	return new anGLFWWindow(title, width, height, callback, resizable);
}
