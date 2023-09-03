#include "anEntryPoint.h"

#include <windows.h>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	return anStartApplication(__argv, __argc);
}
