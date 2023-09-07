#include "anState.h"

anState::anState(anApplication* app)
	: mApplication(app)
{
}

anState::~anState()
{
}

anApplication* anState::GetApplication()
{
	return mApplication;
}
