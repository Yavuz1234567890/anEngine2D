#ifndef AN_TYPES_H_
#define AN_TYPES_H_

typedef unsigned long long anUInt64;
typedef unsigned int anUInt32;
typedef unsigned short anUInt16;
typedef unsigned char anUInt8;

typedef long long anInt64;
typedef signed int anInt32;
typedef signed short anInt16;
typedef signed char anInt8;

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <fstream>
#include <unordered_map>

#include <sol/sol.hpp>

typedef std::string anString;

inline anString anToString(float val)
{
	return std::to_string(val);
}

inline anString anToString(double val)
{
	return std::to_string(val);
}

inline anString anToString(int val)
{
	return std::to_string(val);
}

inline anString anToString(bool val)
{
	return val ? "true" : "false";
}

template<typename T>
using anVector = std::vector<T>;

template<typename K, typename V>
using anMap = std::map<K, V>;

template<typename K, typename V>
using anUnorderedMap = std::unordered_map<K, V>;

template<typename F>
using anFunction = std::function<F>;

typedef std::ifstream anInputFile;
typedef std::ofstream anOutputFile;
typedef std::stringstream anStringStream;

struct anColor
{
	int R;
	int G;
	int B;
	int A;

	anColor()
		: R(0)
		, G(0)
		, B(0)
		, A(255)
	{
	}

	anColor(int r, int g, int b)
		: R(r)
		, G(g)
		, B(b)
		, A(255)
	{
	}

	anColor(int r, int g, int b, int a)
		: R(r)
		, G(g)
		, B(b)
		, A(a)
	{
	}

	static void RegisterLuaAPI(sol::state& state)
	{
		auto col = state.new_usertype<anColor>(
			"anColor",
			sol::constructors<anColor(), anColor(int, int, int), anColor(int, int, int, int)>(),
			"r", &anColor::R,
			"g", &anColor::G,
			"b", &anColor::B,
			"a", &anColor::A
		);
	}
};

#endif
