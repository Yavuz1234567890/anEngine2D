#ifndef AN_SOUND_H_
#define AN_SOUND_H_

#include "anTypes.h"

struct FMOD_SOUND;
struct FMOD_CHANNEL;

namespace anSoundLoopType
{
	enum : anUInt32
	{
		Default = 0,
		None = 1,
		Normal = 2
	};
}

class anSound
{
public:
	anSound();
	~anSound();
	
	static void InitializeFMOD();
	static void ShutdownFMOD();
	void Load(const anString& path);
	void Play();
	void SetPaused(bool paused);
	void Stop();
	bool IsPlaying() const;
	void SetPosition(anUInt32 pos);
	anUInt32 GetPosition() const;
	void SetVolume(float volume);
	void SetLoopType(anUInt32 type);
	anUInt32 GetDuration() const;
	float GetVolume() const;
private:
	FMOD_SOUND* mSound = nullptr;
	FMOD_CHANNEL* mChannel = nullptr;
	unsigned int mDuration = 0;
	float mVolume = 1.0f;
};

#endif
