#include "anInputSystem.h"

anFloat2 anInputSystem::sMousePosition;

bool anInputSystem::sKeys[anKeyLast];
bool anInputSystem::sKeysDown[anKeyLast];
bool anInputSystem::sKeysUp[anKeyLast];

bool anInputSystem::sMouseButtons[8];
bool anInputSystem::sMouseButtonsDown[8];
bool anInputSystem::sMouseButtonsUp[8];

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
	if (event.Type == anEvent::MouseMove)
		sMousePosition = event.MousePosition;

	if (event.Type == anEvent::KeyDown)
	{
		sKeys[event.KeyCode] = true;
		sKeysDown[event.KeyCode] = true;
	}

	if (event.Type == anEvent::KeyUp)
	{
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
