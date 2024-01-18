#include "anScene.h"
#include "anEntity.h"
#include "Core/anUserInputSystem.h"
#include "anPhysicsTypes.h"

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
	mRegistry.destroy(entity);
}

void anScene::EditorUpdate(float dt, anCamera2D& camera)
{
	anRenderer2D::Get().Start(camera);
	
	DrawDrawables();
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

	InitializePhysics();
}

bool anScene::RuntimeUpdate(float dt)
{
	mHasCamera = false;

	{
		auto view = mRegistry.view<anLuaScriptComponent>();
		for (auto entity : view)
		{
			auto script = view.get<anLuaScriptComponent>(entity);
			if (script.Script)
				script.Script->Update(dt);
		}
	}

	UpdatePhysics(dt);

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
				mHasCamera = true;
				cameraView = transform.GetTransformationMatrix();
				mCurrentCameraPosition = transform.Position;
				break;
			}
		}
	}

	if (cam == NULL)
		return false;

	anRenderer2D::Get().Start(*cam, cameraView);
	DrawDrawables();

	return true;
}

void anScene::RuntimeStop()
{
	delete mPhysicsWorld;
	mPhysicsWorld = nullptr;
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

anUInt32 anScene::GetViewportWidth() const
{
	return mViewportWidth;
}

anUInt32 anScene::GetViewportHeight() const
{
	return mViewportHeight;
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

template<typename... Component>
static void CopyComponentIfExistsIgnoreComponent(anEntity dst, anEntity src, const anString& ignore)
{
	([&]()
		{
			if (Component::GetComponentName() != ignore)
			{
				if (src.HasComponent<Component>())
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}
		}(), ...);
}

template<typename... Component>
static void CopyComponentIfExistsIgnoreComponent(anComponentGroup<Component...>, anEntity dst, anEntity src, const anString& ignore)
{
	CopyComponentIfExistsIgnoreComponent<Component...>(dst, src, ignore);
}


anEntity anScene::CopyEntity(anEntity entity, const anString& tag)
{
	anEntity ent = NewEntity(tag);
	CopyComponentIfExistsIgnoreComponent(anAllComponents{}, ent, entity, "TagComponent");
	if (ent.HasComponent<anLuaScriptComponent>())
	{
		auto& comp = ent.GetComponent<anLuaScriptComponent>();

		const anFileSystem::path path = comp.Script->GetPath();
		const anFileSystem::path editorPath = comp.Script->GetEditorPath();

		comp.Script = new anLuaScript();
		comp.Script->LoadScript(path, editorPath);
		comp.Script->Initialize(ent);
	}

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

void anScene::ReloadTextures()
{
	{
		auto view = mRegistry.view<anSpriteRendererComponent>();
		for (auto entity : view)
		{
			auto sprite = view.get<anSpriteRendererComponent>(entity);
			if (sprite.Texture)
				sprite.Texture = anLoadTexture(sprite.Texture->GetPath());
		}
	}
}

void anScene::ReloadAssets()
{
	ReloadScripts();
	ReloadTextures();
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

	newScene->mClearColor = ref->mClearColor;

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

const anFloat2& anScene::GetCurrentCameraPosition() const
{
	return mCurrentCameraPosition;
}

bool anScene::HasCamera() const
{
	return mHasCamera;
}

anColor& anScene::GetClearColor()
{
	return mClearColor;
}

void anScene::RegisterLuaAPI(sol::state& state)
{
	auto scene = state.new_usertype<anScene>(
		"anScene",
		"findEntity", &anScene::FindEntityWithTag,
		"clearColor", &anScene::GetClearColor
	);
}

void anScene::SortDrawableList()
{
	if (!mDrawables.empty())
	{
		for (anUInt64 i = 0; i < mDrawables.size() - 1; i++)
		{
			for (anUInt64 j = 0; j < mDrawables.size() - 1; j++)
			{
				if (mDrawables[j].LayerNumber > mDrawables[j + 1].LayerNumber)
				{
					const anSceneDrawable temp = mDrawables[j];
					mDrawables[j] = mDrawables[j + 1];
					mDrawables[j + 1] = temp;
				}
			}
		}
	}
}

void anScene::DrawDrawables()
{
	{
		auto group = mRegistry.group<anTransformComponent>(entt::get<anSpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<anTransformComponent, anSpriteRendererComponent>(entity);

			anSceneDrawable drawable;
			drawable.Type = anSceneDrawableType::Sprite;
			drawable.Transform = transform;
			drawable.Sprite = sprite;
			drawable.LayerNumber = sprite.LayerNumber;
			mDrawables.push_back(drawable);
		}
	}

	SortDrawableList();

	if (!mDrawables.empty())
	{
		for (auto drawable : mDrawables)
		{
			switch (drawable.Type)
			{
			case anSceneDrawableType::Sprite:
				anRenderer2D::Get().DrawTexture(drawable.Sprite.Texture, drawable.Transform.GetTransformationMatrix(), drawable.Sprite.Color);
				break;
			}
		}

		mDrawables.clear();
	}
}

void anScene::InitializePhysics()
{
	mPhysicsWorld = new b2World({ 0.0f, 9.8f });

	auto view = mRegistry.view<anRigidbodyComponent>();
	for (auto e : view)
	{
		anEntity entity = { e, this };
		auto& transform = entity.GetComponent<anTransformComponent>();
		auto& rb2d = entity.GetComponent<anRigidbodyComponent>();

		b2BodyDef bodyDef;
		bodyDef.type = anPhysicsTypes::RigidbodyTypeToBox2DBodyType(rb2d.Type);
		bodyDef.position.Set(transform.Position.x, transform.Position.y);
		bodyDef.angle = glm::radians(transform.Rotation);

		b2Body* body = mPhysicsWorld->CreateBody(&bodyDef);
		body->SetFixedRotation(rb2d.FixedRotation);
		rb2d.Body = body;

		if (entity.HasComponent<anBoxColliderComponent>())
		{
			auto& bc2d = entity.GetComponent<anBoxColliderComponent>();

			b2PolygonShape boxShape;
			boxShape.SetAsBox(bc2d.Size.x * transform.Size.x, bc2d.Size.y * transform.Size.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;
			fixtureDef.density = bc2d.Density;
			fixtureDef.friction = bc2d.Friction;
			fixtureDef.restitution = bc2d.Restitution;
			fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
			bc2d.Fixture = body->CreateFixture(&fixtureDef);
		}
	}
}

void anScene::UpdatePhysics(float dt)
{
	mPhysicsWorld->Step(dt, 6, 2);

	auto view = mRegistry.view<anRigidbodyComponent>();
	for (auto e : view)
	{
		anEntity entity = { e, this };
		auto& transform = entity.GetComponent<anTransformComponent>();
		auto& rb2d = entity.GetComponent<anRigidbodyComponent>();

		b2Body* body = (b2Body*)rb2d.Body;
		
		const auto& position = body->GetPosition();
		transform.Position.x = position.x;
		transform.Position.y = position.y;
		transform.Rotation = glm::degrees(body->GetAngle());
	}
}
