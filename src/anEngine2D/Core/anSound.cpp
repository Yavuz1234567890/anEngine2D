#include "anSound.h"

#include <fmod.h>
#include <fmod_common.h>

static struct
{
	FMOD_SYSTEM* System = nullptr;
    bool Initialized = false;
} sFMOD;

anSound::anSound()
{
}

anSound::~anSound()
{
}

void anSound::InitializeFMOD()
{
    if (!sFMOD.Initialized)
    {
        unsigned int ver = 0;
        FMOD_System_Create(&sFMOD.System, FMOD_VERSION);
        FMOD_System_GetVersion(sFMOD.System, &ver);
        if (ver < FMOD_VERSION)
        {
        }

        void* extradriverdata = 0;
        FMOD_System_Init(sFMOD.System, 32, FMOD_INIT_NORMAL, extradriverdata);
        sFMOD.Initialized = true;
    }
}

void anSound::ShutdownFMOD() 
{
    FMOD_System_Close(sFMOD.System);
    FMOD_System_Release(sFMOD.System);
}

void anSound::Load(const anString& path)
{
    FMOD_System_CreateSound(sFMOD.System, path.c_str(), FMOD_DEFAULT, 0, &mSound);
    FMOD_Sound_GetLength(mSound, &mDuration, FMOD_TIMEUNIT_MS);
    FMOD_Sound_SetMode(mSound, 0);
}

void anSound::Play()
{
    FMOD_System_PlaySound(sFMOD.System, mSound, 0, false, &mChannel);
    FMOD_Channel_SetVolume(mChannel, mVolume);
}

void anSound::SetPaused(bool paused)
{
    FMOD_Channel_SetPaused(mChannel, paused);
}

void anSound::Stop()
{
    FMOD_Channel_Stop(mChannel);
}

bool anSound::IsPlaying() const
{
    int ip = 0;
    FMOD_Channel_IsPlaying(mChannel, &ip);
    return ip;
}

void anSound::SetPosition(anUInt32 pos)
{
    FMOD_Channel_SetPosition(mChannel, pos, FMOD_TIMEUNIT_MS);
}

anUInt32 anSound::GetPosition() const
{
    anUInt32 position = 0;
    FMOD_Channel_GetPosition(mChannel, &position, FMOD_TIMEUNIT_MS);
    return position;
}

void anSound::SetVolume(float volume)
{
    if (IsPlaying()) FMOD_Channel_SetVolume(mChannel, volume);
}

void anSound::SetLoopType(anUInt32 type)
{
    FMOD_Sound_SetMode(mSound, type);
}

anUInt32 anSound::GetDuration() const
{
    return mDuration;
}

float anSound::GetVolume() const
{
    return mVolume;
}
