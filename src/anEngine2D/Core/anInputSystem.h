#ifndef AN_INPUT_SYSTEM_H_
#define AN_INPUT_SYSTEM_H_

#include "anEvent.h"
#include "anKeyCodes.h"
#include "Math/anFloat2.h"

#include <sol/sol.hpp>

class anInputSystem
{
public:
	static void Initialize();
	static void Update(float dt);
	static void OnEvent(const anEvent& event);

	static bool IsKey(int key);
	static bool IsKeyDown(int key);
	static bool IsKeyUp(int key);
	static bool IsMouseButton(int button);
	static bool IsMouseButtonDown(int button);
	static bool IsMouseButtonUp(int button);
	static anFloat2 GetMousePosition();
	static void SetLocked(bool locked);
	static bool IsLocked();
	static float GetHorizontalAxis();
	static float GetVerticalAxis();
	
	static void RegisterLuaAPI(sol::state& state);
private:
	static bool sKeys[anKeyLast];
	static bool sKeysDown[anKeyLast];
	static bool sKeysUp[anKeyLast];

	static bool sMouseButtons[8];
	static bool sMouseButtonsDown[8];
	static bool sMouseButtonsUp[8];

	static float sHorizontalAxis;
	static float sVerticalAxis;

	static anFloat2 sMousePosition;
	
	static bool sLocked;
};

#endif
