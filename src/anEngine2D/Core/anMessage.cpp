#include "anMessage.h"

#include <windows.h>

void anMessage(const anString& msg)
{
	const anString nlMsg = msg + "\n";
	OutputDebugStringA(nlMsg.c_str());
}

void anShowMessageBox(const anString& msg)
{
	const anString nlMsg = msg + "\n";
	MessageBoxA(NULL, nlMsg.c_str(), "Message", MB_OK);
}
