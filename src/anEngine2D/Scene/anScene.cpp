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
}

bool anScene::RuntimeUpdate(float dt)
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
	int iWidth = (int)width;
	int iHeight = (int)height;

	auto view = mRegistry.view<anCameraComponent>();
	for (auto entity : view)
	{
		auto& cameraComponent = view.get<anCameraComponent>(entity);
		
		cameraComponent.Camera.SetOrtho((float)-iWidth * 0.5f, (float)iWidth * 0.5f, (float)-iHeight * 0.5f, (float)iHeight * 0.5f);
	}
}

entt::registry& anScene::GetRegistry()
{
	return mRegistry;
}
