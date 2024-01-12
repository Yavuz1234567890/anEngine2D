#ifndef AN_SCENE_H_
#define AN_SCENE_H_

class anEntity;

#include "Core/anTypes.h"
#include "Renderer/anRenderer2D.h"

#include <entt.hpp>

class anScene
{
public:
	anScene();
	~anScene();

	anEntity NewEntity(const anString& tag);
	void DestroyEntity(anEntity entity);
	void EditorUpdate(float dt, anCamera2D& camera, anTexture* cameraIcon = nullptr);
	void RuntimeInitialize();
	bool RuntimeUpdate(float dt);
	void OnViewportSize(anUInt32 width, anUInt32 height);
	anEntity FindEntityWithTag(const anString& tag);
	entt::registry& GetRegistry();
	void ReloadScripts();
private:
	entt::registry mRegistry;
	anUInt32 mViewportWidth;
	anUInt32 mViewportHeight;
};

#endif
