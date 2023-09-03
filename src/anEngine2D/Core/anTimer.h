#ifndef AN_TIMER_H_
#define AN_TIMER_H_

#include "anTypes.h"

class anTimer
{
public:
	anTimer();
	~anTimer();

	float GetElapsedTime() const;
	float Tick();
	void Reset();
public:
	static anUInt64 GetCounter();
	static anUInt64 GetFrequency();
private:
	anUInt64 mFrequency;
	anUInt64 mLastTick;
	anUInt64 mFirstTick;
};

#endif
