#include "anMessage.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <tinyfiledialogs.h>

void anMessage(const anString& msg)
{
	const anString nlMsg = msg + "\n";
#ifdef PLATFORM_WINDOWS
	OutputDebugStringA(nlMsg.c_str());
#endif
}

void anShowInformation(const anString& msg)
{
	tinyfd_messageBox("Message", msg.c_str(), "ok", "info", 1);
}
