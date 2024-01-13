#ifndef AN_UUID_H_
#define AN_UUID_H_

#include "anTypes.h"

class anUUID
{
public:
	anUUID();
	anUUID(anUInt64 uuid);
	anUUID(const anUUID&) = default;

	operator uint64_t() const { return mID; }
private:
	anUInt64 mID;
};

namespace std 
{
	template <typename T> struct hash;

	template<>
	struct hash<anUUID>
	{
		std::size_t operator()(const anUUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}

#endif
