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
	int WindowX = 0;
	int WindowY = 0;
	int WindowWidth = 0;
	int WindowHeight = 0;
};

typedef anFunction<void(const anEvent&)> anEventCallback;

bool anIsWindowEvent(anUInt32 event);
bool anIsKeyboardEvent(anUInt32 event);
bool anIsMouseEvent(anUInt32 event);

#endif
