#ifndef AN_INPUT_SYSTEM_H_
#define AN_INPUT_SYSTEM_H_

#include "anEvent.h"
#include "anKeyCodes.h"
#include "Math/anFloat2.h"

class anInputSystem
{
public:
	static void Initialize();
	static void Update(float dt);
	static void OnEvent(const anEvent& event);

	static bool IsKey(int key);
	static bool IsKeyDown(int key);
	static bool IsKeyUp(int key);
	static anFloat2 GetMousePosition();
private:
	static bool sKeys[anKeyLast];
	static bool sKeysDown[anKeyLast];
	static bool sKeysUp[anKeyLast];

	static anFloat2 sMousePosition;
};

#endif
