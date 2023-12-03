#ifndef AN_CONTROLLER_DEVICE_H_
#define AN_CONTROLLER_DEVICE_H_

#include "Math/anFloat2.h"

#define anControllerMax			4

#define anControllerButtonA				0
#define anControllerButtonB				1
#define anControllerButtonY				2
#define anControllerButtonX				3
#define anControllerButtonStart			4
#define anControllerButtonBack			5
#define anControllerButtonLeftShoulder	6
#define anControllerButtonRightShoulder 7
#define anControllerButtonLeftThumb		8
#define anControllerButtonRightThumb	9
#define anControllerButtonUp			10
#define anControllerButtonDown			11
#define anControllerButtonLeft			12
#define anControllerButtonRight			13

#define anControllerButtonMax	14

struct anController
{
	bool IsConnected;

	float LeftTrigger;
	float RightTrigger;

	anFloat2 LeftAxis;
	anFloat2 RightAxis;

	bool ControllerButtons[anControllerButtonMax];
};

class anControllerDevice
{
public:
	anControllerDevice();
	~anControllerDevice();

	void Initialize();
	void Update(float dt);
	void VibrateController(anUInt32 id, float left, float right);
	anController GetController(anUInt32 id) const;
private:
	anController mControllers[anControllerMax];
};

#endif
