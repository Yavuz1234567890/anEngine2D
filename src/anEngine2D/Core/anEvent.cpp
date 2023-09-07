#include "anEvent.h"

bool anIsWindowEvent(anUInt32 event)
{
	return event == anEvent::WindowClose || event == anEvent::WindowMove || event == anEvent::WindowSize;
}

bool anIsKeyboardEvent(anUInt32 event)
{
	return event == anEvent::KeyDown || event == anEvent::KeyUp;
}

bool anIsMouseEvent(anUInt32 event)
{
	return event == anEvent::MouseMove || event == anEvent::MouseWheel || event == anEvent::MouseDown || event == anEvent::MouseUp;
}
