#ifndef AN_ENTITY_H_
#define AN_ENTITY_H_

#include "anScene.h"
#include "Script/anLuaScript.h"
#include "anComponents.h"

#include <entt.hpp>

class anEntity
{
public:
	anEntity();
	anEntity(entt::entity handle, anScene* scene);
	anEntity(const anEntity& other);

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		return mScene->GetRegistry().emplace<T>(mHandle, std::forward<Args>(args)...);;
	}

	template<typename T, typename... Args>
	T& AddOrReplaceComponent(Args&&... args)
	{
		return mScene->GetRegistry().emplace_or_replace<T>(mHandle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent()
	{
		return mScene->GetRegistry().get<T>(mHandle);
	}

	template<typename T>
	bool HasComponent() const
	{
		return mScene->GetRegistry().has<T>(mHandle);
	}

	template<typename T>
	void RemoveComponent()
	{
		mScene->GetRegistry().remove<T>(mHandle);
	}

	operator bool() const;
	operator entt::entity();
	anTransformComponent& GetTransform();
	anUUID& GetUUID();
	anString& GetTag();
	anScene* GetScene();
	void Destroy();
	anEntity Copy(const anString& tag);
	entt::entity GetHandle() const;
	void LookAt(float x, float y);
	bool operator==(const anEntity& ent) const;
	bool operator!=(const anEntity& ent) const;

	//for lua scripting
	bool HasSpriteRenderer() const;
	anSpriteRendererComponent& GetSpriteRenderer();
	bool HasRigidbody() const;
	anRigidbodyComponent& GetRigidbody();
	//-----------------

	static void RegisterLuaAPI(sol::state& state);
private:
	entt::entity mHandle = entt::null;
	anScene* mScene;
};

#endif
