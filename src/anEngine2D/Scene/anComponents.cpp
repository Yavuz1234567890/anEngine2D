#include "anComponents.h"
#include "anPhysicsTypes.h"

void anRigidbodyComponent::ApplyLinearImpulse(const anFloat2& impulse, const anFloat2& point, bool wake)
{
	b2Body* body = (b2Body*)Body;
	body->ApplyLinearImpulse({ impulse.x, impulse.y }, { point.x, point.y }, wake);
}

void anRigidbodyComponent::ApplyLinearImpulseToCenter(const anFloat2& impulse, bool wake)
{
	b2Body* body = (b2Body*)Body;
	body->ApplyLinearImpulseToCenter({ impulse.x, impulse.y }, wake);
}

anFloat2 anRigidbodyComponent::GetLinearVelocity() const
{
	b2Body* body = (b2Body*)Body;
	b2Vec2 linearVel = body->GetLinearVelocity();
	return { linearVel.x, linearVel.y };
}

anUInt32 anRigidbodyComponent::GetType() const
{
	b2Body* body = (b2Body*)Body;
	return anPhysicsTypes::Box2DTypeToRigidbodyType(body->GetType());
}

void anRigidbodyComponent::SetType(anUInt32 type)
{
	b2Body* body = (b2Body*)Body;
	body->SetType(anPhysicsTypes::RigidbodyTypeToBox2DBodyType(type));
}

float anRigidbodyComponent::GetMass() const
{
	b2Body* body = (b2Body*)Body;
	return body->GetMass();
}
