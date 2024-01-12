#include "anInputSystem.h"

anFloat2 anInputSystem::sMousePosition;

bool anInputSystem::sKeys[anKeyLast];
bool anInputSystem::sKeysDown[anKeyLast];
bool anInputSystem::sKeysUp[anKeyLast];

void anInputSystem::Initialize()
{
	for (int i = 0; i < anKeyLast; i++)
	{
		sKeys[i] = false;
		sKeysDown[i] = false;
		sKeysUp[i] = false;
	}
}

void anInputSystem::Update(float dt)
{
	for (int i = 0; i < anKeyLast; i++)
	{
		sKeysDown[i] = false;
		sKeysUp[i] = false;
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

anFloat2 anInputSystem::GetMousePosition()
{
	return sMousePosition;
}
