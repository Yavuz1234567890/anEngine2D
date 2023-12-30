#ifndef AN_IMGUI_DEVICE_H_
#define AN_IMGUI_DEVICE_H_

#include "Core/anWindow.h"

class anImGuiDevice
{
public:
	anImGuiDevice();
	~anImGuiDevice();

	void Initialize(anWindow* window);
	void PrepareCustomStyle();
	void Start();
	void End();
};

#endif
