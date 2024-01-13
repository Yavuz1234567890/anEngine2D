#include "anUUID.h"

#include <random>

static std::random_device sRandomDevice;
static std::mt19937_64 sEngine(sRandomDevice());
static std::uniform_int_distribution<uint64_t> sUniformDistribution;

anUUID::anUUID()
	: mID(sUniformDistribution(sEngine))
{

}

anUUID::anUUID(anUInt64 uuid)
	: mID(uuid)
{

}
