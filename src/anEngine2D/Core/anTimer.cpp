#include "anTimer.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

anTimer::anTimer()
{
	mFrequency = anTimer::GetFrequency();
	mFirstTick = 0;
	mLastTick = 0;
}

anTimer::~anTimer()
{
}

float anTimer::GetElapsedTime() const
{
	return float(double(anTimer::GetCounter() - mFirstTick) / double(mFrequency));
}

float anTimer::Tick()
{
	anUInt64 tick = anTimer::GetCounter();
	const float dt = float(double(tick - mLastTick) / double(mFrequency));
	mLastTick = tick;
	return dt;
}

void anTimer::Reset()
{
	mFirstTick = 0;
	mLastTick = 0;
}

anUInt64 anTimer::GetCounter()
{
	anUInt64 res = 0;
#ifdef PLATFORM_WINDOWS
	QueryPerformanceCounter((LARGE_INTEGER*)&res);
#endif
	return res;
}

anUInt64 anTimer::GetFrequency()
{
	anUInt64 res = 0;
#ifdef PLATFORM_WINDOWS
	QueryPerformanceFrequency((LARGE_INTEGER*)&res);
#endif
	return res;
}
