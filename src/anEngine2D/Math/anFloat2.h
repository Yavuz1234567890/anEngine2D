#ifndef AN_FLOAT2_H_
#define AN_FLOAT2_H_

#include "Core/anTypes.h"

struct anFloat2
{
	float X;
	float Y;

	anFloat2()
		: X(0.0f)
		, Y(0.0f)
	{
	}

	anFloat2(float x, float y)
		: X(x)
		, Y(y)
	{
	}

	anFloat2& operator=(const anFloat2& rhs)
	{
		X = rhs.X;
		Y = rhs.Y;
		return *this;
	}

	anFloat2& operator+=(const anFloat2& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}

	bool operator==(const anFloat2& rhs) const
	{
		return X == rhs.X && Y == rhs.Y;
	}

	bool operator!=(const anFloat2& rhs) const
	{
		return X != rhs.X || Y != rhs.Y;
	}

	anFloat2 operator+(const anFloat2& v) const
	{
		return { X + v.X, Y + v.Y };
	}

	anFloat2 operator-(const anFloat2& v) const
	{
		return { X - v.X, Y - v.Y };
	}

	anFloat2 operator*(const anFloat2& v) const
	{
		return { X * v.X, Y * v.Y };
	}

	anFloat2 operator/(const anFloat2& v) const
	{
		return { X / v.X, Y / v.Y };
	}
	
	float* GetAddress()
	{
		return &X;
	}

	const float* GetAddress() const
	{
		return &X;
	}

	float& operator[](anUInt32 index)
	{
		return GetAddress()[index];
	}

	const float& operator[](anUInt32 index) const
	{
		return GetAddress()[index];
	}
};

#endif
