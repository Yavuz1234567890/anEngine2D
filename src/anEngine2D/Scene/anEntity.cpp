#include "anEntity.h"
#include "Math/anMath.h"

anEntity::anEntity()
	: mHandle(entt::null)
	, mScene(nullptr)
{
}

anEntity::anEntity(entt::entity handle, anScene* scene)
	: mHandle(handle)
	, mScene(scene)
{
}

anEntity::anEntity(const anEntity& other)
	: mHandle(other.mHandle)
	, mScene(other.mScene)
{
}

anEntity::operator bool() const
{
	return mHandle != entt::null;
}

anEntity::operator entt::entity()
{
	return mHandle;
}

anTransformComponent& anEntity::GetTransform()
{
	return GetComponent<anTransformComponent>();
}

anUUID& anEntity::GetUUID()
{
	return GetComponent<anUUIDComponent>().UUID;
}

anString& anEntity::GetTag()
{
	return GetComponent<anTagComponent>().Tag;
}

anScene* anEntity::GetScene()
{
	return mScene;
}

entt::entity anEntity::GetHandle() const
{
	return mHandle;
}

bool anEntity::operator==(const anEntity& ent) const
{
	return mHandle == ent.mHandle && mScene == ent.mScene;
}

bool anEntity::operator!=(const anEntity& ent) const
{
	return mHandle != ent.mHandle || mScene != ent.mScene;
}

void anEntity::Destroy()
{
	mScene->DestroyEntity(*this);
}

anEntity anEntity::Copy(const anString& tag)
{
	return mScene->CopyEntity(*this, tag);
}

void anEntity::LookAt(float x, float y)
{
	anTransformComponent& component = GetTransform();
	component.Rotation = anRadiansToDegrees(atan2f(component.Position.y - y, component.Position.x - x));
}

bool anEntity::HasSpriteRenderer() const
{
	return HasComponent<anSpriteRendererComponent>();
}

anSpriteRendererComponent& anEntity::GetSpriteRenderer()
{
	return GetComponent<anSpriteRendererComponent>();
}

bool anEntity::HasRigidbody() const
{
	return HasComponent<anRigidbodyComponent>();
}

anRigidbodyComponent& anEntity::GetRigidbody()
{
	return GetComponent<anRigidbodyComponent>();
}

void anEntity::RegisterLuaAPI(sol::state& state)
{
	auto entity = state.new_usertype<anEntity>(
		"anEntity",
		"tag", &anEntity::GetTag,
		"transform", &anEntity::GetTransform,
		"scene", &anEntity::GetScene,
		"destroy", &anEntity::Destroy,
		"copy", &anEntity::Copy,
		"lookAt", &anEntity::LookAt,
		"hasSpriteRenderer", &anEntity::HasSpriteRenderer,
		"getSpriteRenderer", &anEntity::GetSpriteRenderer,
		"hasRigidbody", &anEntity::HasRigidbody,
		"getRigidbody", &anEntity::GetRigidbody
	);
}
