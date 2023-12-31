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
	anEntity entity = { mRegistry.create(), this };
	entity.AddComponent<anTransformComponent>();
	auto& tagComp = entity.AddComponent<anTagComponent>();
	tagComp.Tag = tag.empty() ? "New Entity" : tag;
	return entity;
}

void anScene::DestroyEntity(anEntity entity)
{
	mRegistry.destroy(entity.GetHandle());
}

void anScene::EditorUpdate(float dt, anCamera2D& camera)
{
	anRenderer2D::Get().Start(camera);

	{
		auto group = mRegistry.group<anTransformComponent>(entt::get<anSpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<anTransformComponent, anSpriteRendererComponent>(entity);

			anRenderer2D::Get().DrawTexture(sprite.Texture, transform.GetTransformationMatrix(), sprite.Color);
		}
	}

	anRenderer2D::Get().End();
}

void anScene::RuntimeInitialize()
{
}

void anScene::RuntimeUpdate(float dt)
{
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

	if (cam)
	{
		anRenderer2D::Get().Start(*cam, cameraView);

		{
			auto group = mRegistry.group<anTransformComponent>(entt::get<anSpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<anTransformComponent, anSpriteRendererComponent>(entity);

				anRenderer2D::Get().DrawTexture(sprite.Texture, transform.GetTransformationMatrix(), sprite.Color);
			}
		}

		anRenderer2D::Get().End();
	}
}

entt::registry& anScene::GetRegistry()
{
	return mRegistry;
}
