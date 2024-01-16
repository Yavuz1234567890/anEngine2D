#include "anUserInputSystem.h"

anFloat2 anUserInputSystem::sMousePosition;

float anUserInputSystem::sHorizontalAxis = 0.0f;
float anUserInputSystem::sVerticalAxis = 0.0f;

bool anUserInputSystem::sKeys[anKeyLast];
bool anUserInputSystem::sKeysDown[anKeyLast];
bool anUserInputSystem::sKeysUp[anKeyLast];

bool anUserInputSystem::sMouseButtons[8];
bool anUserInputSystem::sMouseButtonsDown[8];
bool anUserInputSystem::sMouseButtonsUp[8];

bool anUserInputSystem::sLocked = false;

void anUserInputSystem::Initialize()
{
	for (int i = 0; i < anKeyLast; i++)
	{
		sKeys[i] = false;
		sKeysDown[i] = false;
		sKeysUp[i] = false;
	}

	for (int i = 0; i < 8; i++)
	{
		sMouseButtons[i] = false;
		sMouseButtonsDown[i] = false;
		sMouseButtonsUp[i] = false;
	}
}

void anUserInputSystem::Update(float dt)
{
	for (int i = 0; i < anKeyLast; i++)
	{
		sKeysDown[i] = false;
		sKeysUp[i] = false;
	}

	for (int i = 0; i < 8; i++)
	{
		sMouseButtonsDown[i] = false;
		sMouseButtonsUp[i] = false;
	}
}

void anUserInputSystem::OnEvent(const anEvent& event)
{
	if (!sLocked)
	{
		if (event.Type == anEvent::KeyDown)
		{
			if (sVerticalAxis == 0.0f)
			{
				if (event.KeyCode == anKeyW)
					sVerticalAxis = -1.0f;

				if (event.KeyCode == anKeyS)
					sVerticalAxis = 1.0f;
			}

			if (sHorizontalAxis == 0.0f)
			{
				if (event.KeyCode == anKeyA)
					sHorizontalAxis = -1.0f;

				if (event.KeyCode == anKeyD)
					sHorizontalAxis = 1.0f;
			}

			sKeys[event.KeyCode] = true;
			sKeysDown[event.KeyCode] = true;
		}

		if (event.Type == anEvent::KeyUp)
		{
			if (event.KeyCode == anKeyW)
			{
				if (sVerticalAxis == -1.0f)
					sVerticalAxis = 0.0f;
			}

			if (event.KeyCode == anKeyS)
			{
				if (sVerticalAxis == 1.0f)
					sVerticalAxis = 0.0f;
			}

			if (event.KeyCode == anKeyA)
			{
				if (sHorizontalAxis == -1.0f)
					sHorizontalAxis = 0.0f;
			}

			if (event.KeyCode == anKeyD)
			{
				if (sHorizontalAxis == 1.0f)
					sHorizontalAxis = 0.0f;
			}

			sKeys[event.KeyCode] = false;
			sKeysUp[event.KeyCode] = true;
		}

		if (event.Type == anEvent::MouseDown)
		{
			sMouseButtons[event.MouseButton] = true;
			sMouseButtonsDown[event.MouseButton] = true;
		}

		if (event.Type == anEvent::MouseUp)
		{
			sMouseButtons[event.MouseButton] = false;
			sMouseButtonsUp[event.MouseButton] = true;
		}
	}
}

bool anUserInputSystem::IsKey(int key)
{
	return sKeys[key];
}

bool anUserInputSystem::IsKeyDown(int key)
{
	return sKeysDown[key];
}

bool anUserInputSystem::IsKeyUp(int key)
{
	return sKeysUp[key];
}

bool anUserInputSystem::IsMouseButton(int button)
{
	return sMouseButtons[button];
}

bool anUserInputSystem::IsMouseButtonDown(int button)
{
	return sMouseButtonsDown[button];
}

bool anUserInputSystem::IsMouseButtonUp(int button)
{
	return sMouseButtonsUp[button];
}

anFloat2 anUserInputSystem::GetMousePosition()
{
	return sMousePosition;
}

void anUserInputSystem::SetMousePosition(const anFloat2& position)
{
	if (!sLocked)
		sMousePosition = position;
}

void anUserInputSystem::SetLocked(bool locked)
{
	sLocked = locked;
	if (locked)
	{
		for (int i = 0; i < anKeyLast; i++)
		{
			sKeys[i] = false;
			sKeysDown[i] = false;
			sKeysUp[i] = false;

			sVerticalAxis = 0.0f;
			sHorizontalAxis = 0.0f;
		}

		for (int i = 0; i < 8; i++)
		{
			sMouseButtons[i] = false;
			sMouseButtonsDown[i] = false;
			sMouseButtonsUp[i] = false;
		}
	}
}

bool anUserInputSystem::IsLocked()
{
	return sLocked;
}

float anUserInputSystem::GetHorizontalAxis()
{
	return sHorizontalAxis;
}

float anUserInputSystem::GetVerticalAxis()
{
	return sVerticalAxis;
}

void anUserInputSystem::RegisterLuaAPI(sol::state& state)
{
	state.set_function("isKey", [&](int key) { return anUserInputSystem::IsKey(key); });
	state.set_function("isKeyDown", [&](int key) { return anUserInputSystem::IsKeyDown(key); });
	state.set_function("isKeyUp", [&](int key) { return anUserInputSystem::IsKeyUp(key); });
	state.set_function("isMouseButton", [&](int button) { return anUserInputSystem::IsMouseButton(button); });
	state.set_function("isMouseButtonDown", [&](int button) { return anUserInputSystem::IsMouseButtonUp(button); });
	state.set_function("isMouseButtonUp", [&](int button) { return anUserInputSystem::IsMouseButtonDown(button); });
	state.set_function("getMousePositionX", [&]() { return anUserInputSystem::GetMousePosition().x; });
	state.set_function("getMousePositionY", [&]() { return anUserInputSystem::GetMousePosition().y; });
	state.set_function("getHorizontalAxis", [&]() { return anUserInputSystem::GetHorizontalAxis(); });
	state.set_function("getVerticalAxis", [&]() { return anUserInputSystem::GetVerticalAxis(); });
}
