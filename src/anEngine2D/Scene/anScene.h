#ifndef AN_SCENE_H_
#define AN_SCENE_H_

class anEntity;

#include "Core/anTypes.h"
#include "Renderer/anRenderer2D.h"
#include "Core/anUUID.h"
#include "anComponents.h"

#include <sol/sol.hpp>
#include <entt.hpp>
#include <box2d/box2d.h>

namespace anSceneDrawableType
{
	enum : anUInt32
	{
		Sprite
	};
}

struct anSceneDrawable
{
	int LayerNumber;
	anTransformComponent Transform;
	anSpriteRendererComponent Sprite;

	anUInt32 Type;
};

class anScene
{
public:
	anScene();
	~anScene();

	anEntity NewEntity(const anString& tag);
	anEntity NewEntity(anUUID uuid, const anString& tag);
	void DestroyEntity(anEntity entity);
	void EditorUpdate(float dt, anCamera2D& camera);
	void RuntimeInitialize();
	bool RuntimeUpdate(float dt);
	void RuntimeStop();
	void OnViewportSize(anUInt32 width, anUInt32 height);
	anUInt32 GetViewportWidth() const;
	anUInt32 GetViewportHeight() const;
	anEntity FindEntityWithTag(const anString& tag);
	entt::registry& GetRegistry();
	void ReloadScripts();
	void ReloadTextures();
	void ReloadAssets();
	anEntity CopyEntity(anEntity entity, const anString& tag);
	const anFloat2& GetCurrentCameraPosition() const;
	bool HasCamera() const;
	anColor& GetClearColor();
	void InitializePhysics();
	void UpdatePhysics(float dt);
	
	static void RegisterLuaAPI(sol::state& state);
	static anScene* Copy(anScene* ref);
private:
	void SortDrawableList();
	void DrawDrawables();
private:
	entt::registry mRegistry;
	anUInt32 mViewportWidth;
	anUInt32 mViewportHeight;

	anUnorderedMap<anUUID, entt::entity> mEntityMap;

	anFloat2 mCurrentCameraPosition;
	bool mHasCamera = false;

	anVector<anSceneDrawable> mDrawables;

	anColor mClearColor;

	b2World* mPhysicsWorld;
};

#endif
