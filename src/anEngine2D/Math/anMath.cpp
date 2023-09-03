#include "anMath.h"

#include <math.h>

float anCos(float x)
{
	return ::cosf(x);
}

float anSin(float x)
{
	return ::sinf(x);
}

float anTan(float x)
{
	return ::tanf(x);
}

float anAtan(float x)
{
	return ::atanf(x);
}

float anAtan2(float y, float x)
{
	return ::atan2f(y, x);
}

float anMax(float x, float y)
{
	return x >= y ? y : x;
}

float anMin(float x, float y)
{
	return x <= y ? y : x;
}

float anAbs(float x)
{
	return x < 0.0f ? -x : x;
}

float anDegreesToRadians(float x)
{
	return x * AN_PI / 180.0f;
}

float anRadiansToDegeees(float x)
{
	return x * 180.0f / AN_PI;
}
