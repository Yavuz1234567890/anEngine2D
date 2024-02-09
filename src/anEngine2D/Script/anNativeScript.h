#ifndef AN_NATIVE_SCRIPT_H_
#define AN_NATIVE_SCRIPT_H_

#include "Scene/anEntity.h"
#include "Core/anEvent.h"

class anNativeScript
{
public:
	anNativeScript(anEntity owner) : Owner(owner) { }
	~anNativeScript() { }

	virtual void Initialize() = 0;
	virtual void Update(float dt) = 0;
	virtual void OnEvent(const anEvent& e) = 0;
protected:
	anEntity Owner;
};

#endif
