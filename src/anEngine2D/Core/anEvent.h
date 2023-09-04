#ifndef AN_EVENT_H_
#define AN_EVENT_H_

#include "anTypes.h"
#include "Math/anFloat2.h"

struct anEvent
{
	enum : anUInt32
	{
		KeyDown,
		KeyUp,
		MouseMove,
		MouseDown,
		MouseUp,
		MouseWheel,
		WindowMove,
		WindowSize,
		WindowClose
	};

	anUInt32 Type = 0;
	anUInt32 KeyCode = 0;
	anUInt32 MouseButton = 0;
	anFloat2 MousePosition;
	anFloat2 MouseScroll;
	anUInt32 WindowX = 0;
	anUInt32 WindowY = 0;
	anUInt32 WindowWidth = 0;
	anUInt32 WindowHeight = 0;
};

typedef anFunction<void(const anEvent&)> anEventCallback;

#endif
