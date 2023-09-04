#include "anFont.h"
#include "anMessage.h"
#include "Device/anGPUCommands.h"

#include <ft2build.h>
#include FT_FREETYPE_H

static struct
{
	FT_Library FreeType;

	bool Initialized = false;
} sFontData;

anFont::anFont()
	: mFontSize(0)
{
}

anFont::~anFont()
{
}

bool anFont::Load(const anString& path, int size)
{
	mPath = path;
	mFontSize = size;

	FT_Face face;
	bool succes = !FT_New_Face(sFontData.FreeType, path.c_str(), 0, &face);
	if (!succes)
	{
		anShowMessageBox("Couldn't find font '" + path + "'");
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, size);
	anPixelStore();
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
			anMessage("Char couldn't find");
            continue;
        }

		anTextureCreationSpecification spec;
		spec.Data = face->glyph->bitmap.buffer;
		spec.Width = face->glyph->bitmap.width;
		spec.Height = face->glyph->bitmap.rows;
		spec.Format = anTextureFormat::Red;
		spec.MinFilter = anTextureParameter::Linear;
		spec.MagFilter = anTextureParameter::Linear;
		spec.WrapS = anTextureParameter::ClampToEdge;
		spec.WrapT = anTextureParameter::ClampToEdge;

		anTexture* texture = new anTexture(spec);
		// for transparent font
		texture->SetTextureSwizzle(anTextureParameter::One, anTextureParameter::One, anTextureParameter::One, anTextureParameter::Red);
        anCharacter character = {
            texture,
            { face->glyph->bitmap.width, face->glyph->bitmap.rows },
            { face->glyph->bitmap_left, face->glyph->bitmap_top },
            face->glyph->advance.x
        };

        mCharacters.insert({ c, character });
    }

    anPixelStore();

    FT_Done_Face(face);
	mIsLoaded = true;
	return true;
}

anCharacter anFont::GetCharacter(char c) const
{
	return mCharacters.at(c);
}

const anString& anFont::GetPath() const
{
	return mPath;
}

int anFont::GetSize() const
{
	return mFontSize;
}

bool anFont::IsLoaded() const
{
	return mIsLoaded;
}

void anFont::Initialize()
{
	if (sFontData.Initialized)
		return;

	if (FT_Init_FreeType(&sFontData.FreeType))
	{
		anShowMessageBox("Error: FreeType couldn't initialized");
		return;
	}

	sFontData.Initialized = true;
}
