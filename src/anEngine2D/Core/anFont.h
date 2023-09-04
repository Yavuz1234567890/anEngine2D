#ifndef AN_FONT_H_
#define AN_FONT_H_

#include "Device/anTexture.h"

struct __int2
{
    int X;
    int Y;
};

struct anCharacter
{
    anTexture* Texture;
    __int2 Size;
    __int2 Bearing;
    anUInt32 Advance;
};

class anFont
{
public:
    anFont();
    ~anFont();

    bool Load(const anString& path, int size);
    anCharacter GetCharacter(char c) const;
    const anString& GetPath() const;
    int GetSize() const;
    bool IsLoaded() const;
public:
    static void Initialize();
private:
    int mFontSize;
    anMap<char, anCharacter> mCharacters;
    anString mPath;
    bool mIsLoaded = false;
};

#endif
