#ifndef AN_LINE_OBJECT_H_
#define AN_LINE_OBJECT_H_

#include "anObject.h"
#include "Math/anFloat2.h"

class anLineObject : public anObject
{
public:
	anLineObject(const anString& name);
	~anLineObject();

	void SetStartPoint(const anFloat2& point);
	void SetEndPoint(const anFloat2& point);
	void SetColor(const anColor& color);
	const anFloat2& GetStartPoint() const;
	const anFloat2& GetEndPoint() const;
	const anColor& GetColor() const;
	anFloat2& GetStartPoint();
	anFloat2& GetEndPoint();
	anColor& GetColor();
private:
	anFloat2 mStartPoint;
	anFloat2 mEndPoint;
	anColor mColor = { 255, 255, 255, 255 };
};

#endif
