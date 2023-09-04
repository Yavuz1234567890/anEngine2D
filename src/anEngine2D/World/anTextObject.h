#ifndef AN_TEXT_OBJECT_H_
#define AN_TEXT_OBJECT_H_

#include "anObject.h"
#include "Core/anFont.h"
#include "Math/anFloat2.h"

class anTextObject : public anObject
{
public:
	anTextObject(const anString& name);
	~anTextObject();

	void SetFont(const anFont& font);
	void SetPosition(const anFloat2& pos);
	void SetColor(const anColor& color);
	void SetText(const anString& text);
	anFont& GetFont();
	anFloat2& GetPosition();
	anColor& GetColor();
	anString& GetText();
	const anFont& GetFont() const;
	const anFloat2& GetPosition() const;
	const anColor& GetColor() const;
	const anString& GetText() const;
private:
	anFont mFont;
	anFloat2 mPosition;
	anColor mColor = { 255, 255, 255, 255 };
	anString mText;
};

#endif
