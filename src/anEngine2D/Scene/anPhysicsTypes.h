#ifndef AN_PHYSICS_TYPES_H_
#define AN_PHYSICS_TYPES_H_

#include "Core/anTypes.h"
#include "anComponents.h"

#include <box2d/box2d.h>

namespace anPhysicsTypes
{
	inline anUInt32 Box2DTypeToRigidbodyType(b2BodyType type)
	{
		switch (type)
		{
		case b2_staticBody: return anRigidbodyType::Static;
		case b2_dynamicBody: return anRigidbodyType::Dynamic;
		case b2_kinematicBody: return anRigidbodyType::Kinematic;
		}

		return anRigidbodyType::Static;
	}

	inline b2BodyType RigidbodyTypeToBox2DBodyType(anUInt32 type)
	{
		switch (type)
		{
		case anRigidbodyType::Static: return b2_staticBody;
		case anRigidbodyType::Dynamic: return b2_dynamicBody;
		case anRigidbodyType::Kinematic: return b2_kinematicBody;
		}

		return b2_staticBody;
	}
}

#endif
