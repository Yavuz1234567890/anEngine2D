#ifndef AN_WINDOW_H_
#define AN_WINDOW_H_

#include "anTypes.h"

class anWindow
{
public:
	anWindow(const anString& title, anUInt32 width, anUInt32 height)
		: mTitle(title)
		, mStartWidth(width)
		, mStartHeight(height)
		, mIsVSync(true)
	{ 
	}
		
	~anWindow() 
	{
	}

	virtual void Present() = 0;
	virtual void Close() = 0;
	virtual bool IsRunning() const = 0;
	virtual void MakeFullscreen() = 0;
	virtual void SetTitle(const anString& title) = 0;
	virtual void SetVSync(bool vsync) = 0;

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
		return mStartWidth;
	}

	bool IsVSync() const
	{
		return mIsVSync;
	}

protected:
	anString mTitle;
	anUInt32 mStartWidth;
	anUInt32 mStartHeight;
	bool mIsVSync;
};

anWindow* anCreateWindow(const anString& title, anUInt32 width, anUInt32 height);

#endif
