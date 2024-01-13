#include "anScene.h"
#include "anEntity.h"

anScene::anScene()
{
}

anScene::~anScene()
{
}

anEntity anScene::NewEntity(const anString& tag)
{
	return NewEntity(anUUID(), tag);
}

anEntity anScene::NewEntity(anUUID uuid, const anString& tag)
{
	anEntity entity = { mRegistry.create(), this };
	entity.AddComponent<anTransformComponent>();
	entity.AddComponent<anUUIDComponent>(uuid);
	auto& tagComp = entity.AddComponent<anTagComponent>();
	tagComp.Tag = tag.empty() ? "New Entity" : tag;

	mEntityMap[uuid] = entity;
	return entity;
}

void anScene::DestroyEntity(anEntity entity)
{
	mEntityMap.erase(entity.GetUUID());
	mRegistry.destroy(entity.GetHandle());
}

void anScene::EditorUpdate(float dt, anCamera2D& camera, anTexture* cameraIcon)
{
	anRenderer2D::Get().Start(camera);

	{
		auto group = mRegistry.group<anTransformComponent>(entt::get<anSpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<anTransformComponent, anSpriteRendererComponent>(entity);

			anRenderer2D::Get().DrawTexture(sprite.Texture, transform.GetTransformationMatrix(), sprite.Color);
		}

		auto view = mRegistry.view<anTransformComponent, anCameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<anTransformComponent, anCameraComponent>(entity);

			int camW = (int)cameraIcon->GetWidth();
			int camH = (int)cameraIcon->GetHeight();
			anRenderer2D::Get().DrawTexture(cameraIcon, transform.Position, { (float)camW, (float)camH }, { 255, 255, 255 });
		}
	}
}

void anScene::RuntimeInitialize()
{
	{
		auto view = mRegistry.view<anLuaScriptComponent>();
		for (auto entity : view)
		{
			auto script = view.get<anLuaScriptComponent>(entity);
			if (script.Script)
				script.Script->Initialize({ entity, this });
		}
	}
}

bool anScene::RuntimeUpdate(float dt)
{
	{
		auto view = mRegistry.view<anLuaScriptComponent>();
		for (auto entity : view)
		{
			auto script = view.get<anLuaScriptComponent>(entity);
			if (script.Script)
				script.Script->Update(dt);
		}
	}

	anCamera2D* cam = nullptr;
	anMatrix4 cameraView;
	{
		auto view = mRegistry.view<anTransformComponent, anCameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<anTransformComponent, anCameraComponent>(entity);

			if (camera.Current)
			{
				cam = &camera.Camera;
				cameraView = transform.GetTransformationMatrix();
				break;
			}
		}
	}

	if (cam == NULL)
		return false;

	anRenderer2D::Get().Start(*cam, cameraView);

	{
		auto group = mRegistry.group<anTransformComponent>(entt::get<anSpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<anTransformComponent, anSpriteRendererComponent>(entity);

			anRenderer2D::Get().DrawTexture(sprite.Texture, transform.GetTransformationMatrix(), sprite.Color);
		}
	}

	return true;
}

void anScene::OnViewportSize(anUInt32 width, anUInt32 height)
{
	mViewportWidth = width;
	mViewportHeight = height;

	int iWidth = (int)width;
	int iHeight = (int)height;

	auto view = mRegistry.view<anCameraComponent>();
	for (auto entity : view)
	{
		auto& cameraComponent = view.get<anCameraComponent>(entity);
		
		cameraComponent.Camera.SetOrtho((float)-iWidth * 0.5f, (float)iWidth * 0.5f, (float)-iHeight * 0.5f, (float)iHeight * 0.5f);
	}
}

anEntity anScene::FindEntityWithTag(const anString& tag)
{
	auto view = mRegistry.view<anTagComponent>();
	for (auto entity : view)
	{
		const anTagComponent& tc = view.get<anTagComponent>(entity);
		if (tc.Tag == tag)
			return anEntity{ entity, this };
	}
	return {};
}

entt::registry& anScene::GetRegistry()
{
	return mRegistry;
}

template<typename... Component>
static void CopyComponentIfExists(anEntity dst, anEntity src)
{
	([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
}

template<typename... Component>
static void CopyComponentIfExists(anComponentGroup<Component...>, anEntity dst, anEntity src)
{
	CopyComponentIfExists<Component...>(dst, src);
}

anEntity anScene::CopyEntity(anEntity entity)
{
	const anString tag = entity.GetTag();
	anEntity ent = NewEntity(tag);
	CopyComponentIfExists(anAllComponents{}, ent, entity);
	return ent;
}

void anScene::ReloadScripts()
{
	{
		auto view = mRegistry.view<anLuaScriptComponent>();
		for (auto entity : view)
		{
			auto script = view.get<anLuaScriptComponent>(entity);
			if (script.Script)
				script.Script->LoadScript(script.Script->GetPath(), script.Script->GetEditorPath());
		}
	}
}

template<typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const anUnorderedMap<anUUID, entt::entity>& enttMap)
{
	([&]()
		{
			auto view = src.view<Component>();
			for (auto srcEntity : view)
			{
				entt::entity dstEntity = enttMap.at(src.get<anUUIDComponent>(srcEntity).UUID);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			}
		}(), ...);
}

template<typename... Component>
static void CopyComponent(anComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const anUnorderedMap<anUUID, entt::entity>& enttMap)
{
	CopyComponent<Component...>(dst, src, enttMap);
}

anScene* anScene::Copy(anScene* ref)
{
	anScene* newScene = new anScene();

	newScene->mViewportWidth = ref->mViewportWidth;
	newScene->mViewportHeight = ref->mViewportHeight;

	auto& srcSceneRegistry = ref->mRegistry;
	auto& dstSceneRegistry = newScene->mRegistry;
	anUnorderedMap<anUUID, entt::entity> enttMap;

	auto idView = srcSceneRegistry.view<anUUIDComponent>();
	for (auto e : idView)
	{
		anUUID uuid = srcSceneRegistry.get<anUUIDComponent>(e).UUID;
		const auto& name = srcSceneRegistry.get<anTagComponent>(e).Tag;
		anEntity newEntity = newScene->NewEntity(uuid, name);
		enttMap[uuid] = (entt::entity)newEntity;
	}

	CopyComponent(anAllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

	return newScene;
}
