#include "anInputSystem.h"

anFloat2 anInputSystem::sMousePosition;

float anInputSystem::sHorizontalAxis = 0.0f;
float anInputSystem::sVerticalAxis = 0.0f;

bool anInputSystem::sKeys[anKeyLast];
bool anInputSystem::sKeysDown[anKeyLast];
bool anInputSystem::sKeysUp[anKeyLast];

bool anInputSystem::sMouseButtons[8];
bool anInputSystem::sMouseButtonsDown[8];
bool anInputSystem::sMouseButtonsUp[8];

bool anInputSystem::sLocked = false;

void anInputSystem::Initialize()
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

void anInputSystem::Update(float dt)
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

void anInputSystem::OnEvent(const anEvent& event)
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

		if (event.Type == anEvent::MouseMove)
		{
			sMousePosition = event.MousePosition;
		}
	}
}

bool anInputSystem::IsKey(int key)
{
	return sKeys[key];
}

bool anInputSystem::IsKeyDown(int key)
{
	return sKeysDown[key];
}

bool anInputSystem::IsKeyUp(int key)
{
	return sKeysUp[key];
}

bool anInputSystem::IsMouseButton(int button)
{
	return sMouseButtons[button];
}

bool anInputSystem::IsMouseButtonDown(int button)
{
	return sMouseButtonsDown[button];
}

bool anInputSystem::IsMouseButtonUp(int button)
{
	return sMouseButtonsUp[button];
}

anFloat2 anInputSystem::GetMousePosition()
{
	return sMousePosition;
}

void anInputSystem::SetLocked(bool locked)
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

bool anInputSystem::IsLocked()
{
	return sLocked;
}

float anInputSystem::GetHorizontalAxis()
{
	return sHorizontalAxis;
}

float anInputSystem::GetVerticalAxis()
{
	return sVerticalAxis;
}

void anInputSystem::RegisterLuaAPI(sol::state& state)
{
	state.set_function("isKey", [&](int key) { return anInputSystem::IsKey(key); });
	state.set_function("isKeyDown", [&](int key) { return anInputSystem::IsKeyDown(key); });
	state.set_function("isKeyUp", [&](int key) { return anInputSystem::IsKeyUp(key); });
	state.set_function("isMouseButton", [&](int button) { return anInputSystem::IsMouseButton(button); });
	state.set_function("isMouseButtonDown", [&](int button) { return anInputSystem::IsMouseButtonUp(button); });
	state.set_function("isMouseButtonUp", [&](int button) { return anInputSystem::IsMouseButtonDown(button); });
	state.set_function("getMousePositionX", [&]() { return anInputSystem::GetMousePosition().x; });
	state.set_function("getMousePositionY", [&]() { return anInputSystem::GetMousePosition().y; });
	state.set_function("getHorizontalAxis", [&]() { return anInputSystem::GetHorizontalAxis(); });
	state.set_function("getVerticalAxis", [&]() { return anInputSystem::GetVerticalAxis(); });
}
