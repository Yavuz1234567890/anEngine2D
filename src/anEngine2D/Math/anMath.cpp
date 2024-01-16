#include "anMath.h"
#include "Core/anTimer.h"
#include "anFloat4.h"

#include <math.h>
#include <stdlib.h>

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

float anRadiansToDegrees(float x)
{
	return x * 180.0f / AN_PI;
}

void anInitializeRandomDevice()
{
	srand(anUInt32(anTimer::GetCounter()));
}

int anRand()
{
	return rand();
}

int anRand(int max)
{
	return rand() % max;
}

int anRand(int min, int max)
{
	return min + rand() % (max - min);
}

float anRandf(float max)
{
	return (float(rand()) / RAND_MAX) * max;
}

float anRandf(float min, float max)
{
	return min + (float(rand()) / RAND_MAX) * (max - min);
}

namespace anMath
{
	void RegisterLuaAPI(sol::state& state)
	{
		auto multOverloadsFloat2 =
			sol::overload([](const anFloat2& v1, const anFloat2& v2) -> anFloat2 { return v1 * v2; },
				[](const anFloat2& v1, float f) -> anFloat2 { return v1 * f; },
				[](float f, const anFloat2& v1) -> anFloat2 { return f * v1; });

		auto divOverloadsFloat2 = sol::overload([](const anFloat2& v1, const anFloat2& v2) -> anFloat2 { return v1 / v2; },
			[](const anFloat2& v1, float f) -> anFloat2 { return v1 / f; },
			[](float f, const anFloat2& v1) -> anFloat2 { return f / v1; });

		auto addOverloadsFloat2 =
			sol::overload([](const anFloat2& v1, const anFloat2& v2) -> anFloat2 { return v1 + v2; });

		auto subtractOverloadsFloat2 =
			sol::overload([](const anFloat2& v1, const anFloat2& v2) -> anFloat2 { return v1 - v2; });

		auto multOverloadsFloat3 =
			sol::overload([](const anFloat3& v1, const anFloat3& v2) -> anFloat3 { return v1 * v2; },
				[](const anFloat3& v1, float f) -> anFloat3 { return v1 * f; },
				[](float f, const anFloat3& v1) -> anFloat3 { return f * v1; });

		auto divOverloadsFloat3 = sol::overload([](const anFloat3& v1, const anFloat3& v2) -> anFloat3 { return v1 / v2; },
			[](const anFloat3& v1, float f) -> anFloat3 { return v1 / f; },
			[](float f, const anFloat3& v1) -> anFloat3 { return f / v1; });

		auto addOverloadsFloat3 =
			sol::overload([](const anFloat3& v1, const anFloat3& v2) -> anFloat3 { return v1 + v2; });

		auto subtractOverloadsFloat3 =
			sol::overload([](const anFloat3& v1, const anFloat3& v2) -> anFloat3 { return v1 - v2; });

		auto multOverloadsFloat4 =
			sol::overload([](const anFloat4& v1, const anFloat4& v2) -> anFloat4 { return v1 * v2; },
				[](const anFloat4& v1, float f) -> anFloat4 { return v1 * f; },
				[](float f, const anFloat4& v1) -> anFloat4 { return f * v1; });

		auto divOverloadsFloat4 = sol::overload([](const anFloat4& v1, const anFloat4& v2) -> anFloat4 { return v1 / v2; },
			[](const anFloat4& v1, float f) -> anFloat4 { return v1 / f; },
			[](float f, const anFloat4& v1) -> anFloat4 { return f / v1; });

		auto addOverloadsFloat4 =
			sol::overload([](const anFloat4& v1, const anFloat4& v2) -> anFloat4 { return v1 + v2; });

		auto subtractOverloadsFloat4 =
			sol::overload([](const anFloat4& v1, const anFloat4& v2) -> anFloat4 { return v1 - v2; });

		auto float2 = state.new_usertype<anFloat2>(
			"anFloat2",
			sol::constructors<anFloat2(), anFloat2(anFloat2), anFloat2(float), anFloat2(float, float)>(),
			"x", &anFloat2::x,
			"y", &anFloat2::y
		);

		auto float3 = state.new_usertype<anFloat3>(
			"anFloat3",
			sol::constructors<anFloat3(), anFloat3(anFloat3), anFloat3(float), anFloat3(float, float, float)>(),
			"x", &anFloat3::x,
			"y", &anFloat3::y,
			"z", &anFloat3::z
		);

		auto float4 = state.new_usertype<anFloat4>(
			"anFloat4",
			sol::constructors<anFloat4(), anFloat4(anFloat4), anFloat4(float), anFloat4(float, float, float, float)>(),
			"x", &anFloat4::x,
			"y", &anFloat4::y,
			"z", &anFloat4::z,
			"w", &anFloat4::w
		);
	}
}
