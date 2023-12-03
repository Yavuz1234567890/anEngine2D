#include "anControllerDevice.h"
#include "anMessage.h"

#ifdef PLATFORM_WINDOWS

#include <windows.h>
#include <xinput.h>

typedef decltype(XInputGetState)* GetStateFn;
typedef decltype(XInputSetState)* SetStateFn;

static GetStateFn sXInputGetState = NULL;
static SetStateFn sXInputSetState = NULL;
static XINPUT_STATE sState;
static HMODULE sXInputLibHandle = NULL;
static XINPUT_VIBRATION sVibration;
static bool sXInputInitialized = false;

#endif

anControllerDevice::anControllerDevice()
{
}

anControllerDevice::~anControllerDevice()
{
}

void anControllerDevice::Initialize()
{
#ifdef PLATFORM_WINDOWS

	if (!sXInputInitialized)
	{
		sXInputLibHandle = LoadLibraryW(L"Xinput9_1_0.dll");
		if (sXInputLibHandle)
		{
			sXInputGetState = (GetStateFn)GetProcAddress(sXInputLibHandle, "XInputGetState");
			sXInputSetState = (SetStateFn)GetProcAddress(sXInputLibHandle, "XInputSetState");
			if (!sXInputGetState || !sXInputSetState)
			{
				FreeLibrary(sXInputLibHandle);
				return;
			}
		}
	
		sXInputInitialized = true;
	}

	DWORD dwResult = 0;
	for (int i = 0; i < anControllerMax; i++)
	{
		ZeroMemory(&sState, sizeof(XINPUT_STATE));
		dwResult = sXInputGetState(i, &sState);
		mControllers[i].IsConnected = (dwResult == ERROR_SUCCESS);
	}

#endif
}

static anFloat2 _NormalizeAxis(float x, float y)
{
	float base = sqrtf(x * x + y * y);
	float normX = x / base;
	float normY = y / base;
	float normBase = 0.0f;
	if (base > 7849)
	{
		if (base > 32767)
		{
			base = 32767;
		}
		base -= 7849;
		normBase = base / (32767 - 7849);
	}
	else
	{
		base = 0.0f;
		normBase = 0.0f;
	}
	return { normX * normBase, normY * normBase };
}

void anControllerDevice::Update(float dt)
{
#ifdef PLATFORM_WINDOWS
	
	DWORD dwResult = 0;
	for (int i = 0; i < anControllerMax; i++)
	{
		ZeroMemory(&sState, sizeof(XINPUT_STATE));
		dwResult = sXInputGetState(i, &sState);
		mControllers[i].IsConnected = (dwResult == ERROR_SUCCESS);
		if (mControllers[i].IsConnected)
		{
			mControllers[i].LeftTrigger = sState.Gamepad.bLeftTrigger / 255.0f;
			mControllers[i].RightTrigger = sState.Gamepad.bRightTrigger / 255.0f;
			mControllers[i].LeftAxis = _NormalizeAxis(sState.Gamepad.sThumbLX, sState.Gamepad.sThumbLY);
			mControllers[i].RightAxis = _NormalizeAxis(sState.Gamepad.sThumbRX, sState.Gamepad.sThumbRY);

			mControllers[i].ControllerButtons[anControllerButtonA] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
			mControllers[i].ControllerButtons[anControllerButtonB] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_B;
			mControllers[i].ControllerButtons[anControllerButtonX] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_X;
			mControllers[i].ControllerButtons[anControllerButtonY] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
			mControllers[i].ControllerButtons[anControllerButtonStart] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_START;
			mControllers[i].ControllerButtons[anControllerButtonBack] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
			mControllers[i].ControllerButtons[anControllerButtonLeftShoulder] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
			mControllers[i].ControllerButtons[anControllerButtonRightShoulder] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
			mControllers[i].ControllerButtons[anControllerButtonLeftThumb] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
			mControllers[i].ControllerButtons[anControllerButtonRightThumb] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
			mControllers[i].ControllerButtons[anControllerButtonUp] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
			mControllers[i].ControllerButtons[anControllerButtonDown] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
			mControllers[i].ControllerButtons[anControllerButtonLeft] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
			mControllers[i].ControllerButtons[anControllerButtonRight] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		}
	}
	
#endif
}

void anControllerDevice::VibrateController(anUInt32 id, float left, float right)
{
	int idx = (int)id;
	if (mControllers[idx].IsConnected)
	{
#ifdef PLATFORM_WINDOWS

		ZeroMemory(&sVibration, sizeof(XINPUT_VIBRATION));

		int leftVib = (int)(left * 65535.0f);
		int rightVib = (int)(right * 65535.0f);

		sVibration.wLeftMotorSpeed = leftVib;
		sVibration.wRightMotorSpeed = rightVib;
		
		sXInputSetState(idx, &sVibration);

#endif
	}
}

anController anControllerDevice::GetController(anUInt32 id) const
{
	return mControllers[id];
}
