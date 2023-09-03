#ifndef AN_FLOAT3_H_
#define AN_FLOAT3_H_

#include "Core/anTypes.h"
#include "anFloat2.h"

struct anFloat3
{
	float X;
	float Y;
	float Z;

	anFloat3()
		: X(0.0f)
		, Y(0.0f)
		, Z(0.0f)
	{
	}

	anFloat3(float x, float y, float z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}

	anFloat3& operator=(const anFloat3& rhs)
	{
		X = rhs.X;
		Y = rhs.Y;
		Z = rhs.Z;
		return *this;
	}

	anFloat3& operator+=(const anFloat3& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		Z += rhs.Z;
		return *this;
	}

	bool operator==(const anFloat3& rhs) const
	{
		return X == rhs.X && Y == rhs.Y && Z == rhs.Z;
	}

	bool operator!=(const anFloat3& rhs) const
	{
		return X != rhs.X || Y != rhs.Y || Z != rhs.Z;
	}

	anFloat3 operator+(const anFloat3& v) const
	{
		return { X + v.X, Y + v.Y, Z + v.Z };
	}

	anFloat3 operator-(const anFloat3& v) const
	{
		return { X - v.X, Y - v.Y, Z - v.Z };
	}

	anFloat3 operator*(const anFloat3& v) const
	{
		return { X * v.X, Y * v.Y, Z * v.Z };
	}

	anFloat3 operator/(const anFloat3& v) const
	{
		return { X / v.X, Y / v.Y, Z / v.Z };
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
