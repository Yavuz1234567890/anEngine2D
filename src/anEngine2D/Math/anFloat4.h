#ifndef AN_FLOAT4_H_
#define AN_FLOAT4_H_

#include "Core/anTypes.h"
#include "anFloat3.h"

struct anFloat4
{
	float X;
	float Y;
	float Z;
	float W;

	anFloat4()
		: X(0.0f)
		, Y(0.0f)
		, Z(0.0f)
		, W(0.0f)
	{
	}

	anFloat4(float x, float y, float z, float w)
		: X(x)
		, Y(y)
		, Z(z)
		, W(w)
	{
	}

	anFloat4& operator=(const anFloat4& rhs)
	{
		X = rhs.X;
		Y = rhs.Y;
		Z = rhs.Z;
		W = rhs.W;
		return *this;
	}

	anFloat4& operator+=(const anFloat4& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		Z += rhs.Z;
		W += rhs.W;
		return *this;
	}

	bool operator==(const anFloat4& rhs) const
	{
		return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
	}

	bool operator!=(const anFloat4& rhs) const
	{
		return X != rhs.X || Y != rhs.Y || Z != rhs.Z || W != rhs.W;
	}

	anFloat4 operator+(const anFloat4& v) const
	{
		return { X + v.X, Y + v.Y, Z + v.Z, W + v.W };
	}

	anFloat4 operator-(const anFloat4& v) const
	{
		return { X - v.X, Y - v.Y, Z - v.Z, W - v.W };
	}

	anFloat4 operator*(const anFloat4& v) const
	{
		return { X * v.X, Y * v.Y, Z * v.Z, W * v.W };
	}

	anFloat4 operator/(const anFloat4& v) const
	{
		return { X / v.X, Y / v.Y, Z / v.Z, W / v.W };
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
