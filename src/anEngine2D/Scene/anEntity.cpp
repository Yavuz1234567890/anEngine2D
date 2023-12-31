#include "anEntity.h"

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

const anString anEntity::GetTag()
{
	return GetComponent<anTagComponent>().Tag;
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
