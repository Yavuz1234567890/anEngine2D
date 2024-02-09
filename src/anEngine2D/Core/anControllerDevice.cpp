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

anController anControllerDevice::sControllers[anControllerMax];

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
		sControllers[i].IsConnected = (dwResult == ERROR_SUCCESS);
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
		sControllers[i].IsConnected = (dwResult == ERROR_SUCCESS);
		if (sControllers[i].IsConnected)
		{
			sControllers[i].LeftTrigger = sState.Gamepad.bLeftTrigger / 255.0f;
			sControllers[i].RightTrigger = sState.Gamepad.bRightTrigger / 255.0f;
			sControllers[i].LeftAxis = _NormalizeAxis(sState.Gamepad.sThumbLX, sState.Gamepad.sThumbLY);
			sControllers[i].RightAxis = _NormalizeAxis(sState.Gamepad.sThumbRX, sState.Gamepad.sThumbRY);

			sControllers[i].ControllerButtons[anControllerButtonA] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
			sControllers[i].ControllerButtons[anControllerButtonB] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_B;
			sControllers[i].ControllerButtons[anControllerButtonX] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_X;
			sControllers[i].ControllerButtons[anControllerButtonY] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
			sControllers[i].ControllerButtons[anControllerButtonStart] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_START;
			sControllers[i].ControllerButtons[anControllerButtonBack] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
			sControllers[i].ControllerButtons[anControllerButtonLeftShoulder] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
			sControllers[i].ControllerButtons[anControllerButtonRightShoulder] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
			sControllers[i].ControllerButtons[anControllerButtonLeftThumb] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
			sControllers[i].ControllerButtons[anControllerButtonRightThumb] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
			sControllers[i].ControllerButtons[anControllerButtonUp] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
			sControllers[i].ControllerButtons[anControllerButtonDown] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
			sControllers[i].ControllerButtons[anControllerButtonLeft] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
			sControllers[i].ControllerButtons[anControllerButtonRight] = sState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		}
	}
	
#endif
}

void anControllerDevice::VibrateController(anUInt32 id, float left, float right)
{
	int idx = (int)id;
	if (sControllers[idx].IsConnected)
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

anController anControllerDevice::GetController(anUInt32 id)
{
	return sControllers[id];
}

float anControllerDevice::GetControllerLeftTrigger(anUInt32 id)
{
	return sControllers[id].LeftTrigger;
}

float anControllerDevice::GetControllerRightTrigger(anUInt32 id)
{
	return sControllers[id].RightTrigger;
}

anFloat2 anControllerDevice::GetControllerLeftAxis(anUInt32 id)
{
	return sControllers[id].LeftAxis;
}

anFloat2 anControllerDevice::GetControllerRightAxis(anUInt32 id)
{
	return sControllers[id].RightAxis;
}

bool anControllerDevice::GetControllerButton(anUInt32 id, int button)
{
	return sControllers[id].ControllerButtons[button];
}

bool anControllerDevice::IsControllerConnected(anUInt32 id)
{
	return sControllers[id].IsConnected;
}

void anControllerDevice::RegisterLuaAPI(sol::state& state)
{
	state.set_function("vibrateController", &anControllerDevice::VibrateController);
	state.set_function("getControllerLeftTrigger", &anControllerDevice::GetControllerLeftTrigger);
	state.set_function("getControllerRightTrigger", &anControllerDevice::GetControllerRightTrigger);
	state.set_function("getControllerLeftAxis", &anControllerDevice::GetControllerLeftAxis);
	state.set_function("getControllerRightAxis", &anControllerDevice::GetControllerRightAxis);
	state.set_function("getControllerButton", &anControllerDevice::GetControllerButton);
	state.set_function("isControllerConnected", &anControllerDevice::IsControllerConnected);
}
