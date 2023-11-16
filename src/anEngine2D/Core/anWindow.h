#ifndef AN_WINDOW_H_
#define AN_WINDOW_H_

#include "anTypes.h"
#include "anEvent.h"
#include "Device/anTexture.h"

#include <stb_image.h>

class anWindow
{
public:
	anWindow(const anString& title, anUInt32 width, anUInt32 height, const anEventCallback& callback, bool isResizable = true)
		: mTitle(title)
		, mStartWidth(width)
		, mStartHeight(height)
		, mIsVSync(true)
		, mIsRunning(true)
		, mOnEventCallback(callback)
	{ 
	}
		
	~anWindow() 
	{
	}

	virtual void Present() = 0;
	virtual void SetTitle(const anString& title) = 0;
	virtual void SetVSync(bool vsync) = 0;
	virtual void SetWindowIcon(const void* data, int width, int height) = 0;
	
	void SetWindowIcon(const anString& path)
	{
		int width = 0;
		int height = 0;
		int channels = 0;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

		SetWindowIcon(data, width, height);

		stbi_image_free(data);
	}

	void Close()
	{
		mIsRunning = false;
	}

	bool IsRunning() const
	{
		return mIsRunning;
	}

	const anString& GetTitle() const
	{
		return mTitle;
	}

	anUInt32 GetStartWidth() const
	{
		return mStartWidth;
	}

	anUInt32 GetStartHeight() const
	{
		return mStartHeight;
	}

	bool IsVSync() const
	{
		return mIsVSync;
	}

	void SetEventCallback(const anEventCallback& callback)
	{
		mOnEventCallback = callback;
	}

	void OnKeyDown(anUInt32 keyCode)
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::KeyDown;
		event.KeyCode = keyCode;
		CallEventCallback(event);
	}

	void OnKeyUp(anUInt32 keyCode)
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::KeyUp;
		event.KeyCode = keyCode;
		CallEventCallback(event);
	}

	void OnMouseButtonDown(anUInt32 mouseButton, const anFloat2& pos)
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::MouseDown;
		event.MouseButton = mouseButton;
		CallEventCallback(event);
	}

	void OnMouseButtonUp(anUInt32 mouseButton, const anFloat2& pos)
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::MouseUp;
		event.MouseButton = mouseButton;
		CallEventCallback(event);
	}

	void OnMouseMove(const anFloat2& pos)
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::MouseMove;
		event.MousePosition = pos;
		CallEventCallback(event);
	}

	void OnMouseWheel(const anFloat2& scroll)
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::MouseWheel;
		event.MouseScroll = scroll;
		CallEventCallback(event);
	}

	void OnWindowMove(int x, int y)
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::WindowMove;
		event.WindowX = x;
		event.WindowY = y;
		CallEventCallback(event);
	}

	void OnWindowSize(int width, int height)
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::WindowSize;
		event.WindowWidth = width;
		event.WindowHeight = height;
		CallEventCallback(event);
	}

	void OnWindowClose()
	{
		anEvent event;
		memset(&event, 0, sizeof(anEvent));
		event.Type = anEvent::WindowClose;
		CallEventCallback(event);

		mIsRunning = false;
	}

protected:
	void CallEventCallback(const anEvent& event)
	{
		if (mOnEventCallback)
			mOnEventCallback(event);
	}

protected:
	anString mTitle;
	anUInt32 mStartWidth;
	anUInt32 mStartHeight;
	bool mIsVSync;
	bool mIsRunning;
	anEventCallback mOnEventCallback;
};

anWindow* anCreateWindow(const anString& title, anUInt32 width, anUInt32 height, const anEventCallback& callback, bool resizable);

#endif
