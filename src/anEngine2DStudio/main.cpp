#include "anEngine2DEditor.h"

int anStartApplication(char** args, int argc)
{
	anEngine2DApplication* app = new anEngine2DApplication();
	app->Start();

	return 0;
}
