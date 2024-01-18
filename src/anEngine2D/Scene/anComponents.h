#ifndef AN_COMPONENTS_H_
#define AN_COMPONENTS_H_

#include "Core/anTypes.h"
#include "Math/anMatrix4.h"
#include "Device/anTexture.h"
#include "Renderer/anCamera2D.h"
#include "Core/anUUID.h"
#include "Core/anSound.h"

class anLuaScript;

struct anUUIDComponent
{
	anUUID UUID;

	anUUIDComponent() = default;
	anUUIDComponent(const anUUIDComponent&) = default;

	static anString GetComponentName() { return "UUIDComponent"; }
};

struct anTagComponent
{
	anString Tag;

	anTagComponent() = default;
	anTagComponent(const anTagComponent&) = default;
	anTagComponent(const anString& tag)
		: Tag(tag) {}

	static anString GetComponentName() { return "TagComponent"; }
};

struct anTransformComponent
{
	anFloat2 Position = { 0.0f, 0.0f };
	anFloat2 Size = { 1.0f, 1.0f };
	float Rotation = 0.0f;

	void IncreasePosition(float x, float y)
	{
		Position += anFloat2(x, y);
	}

	void IncreaseSize(float x, float y)
	{
		Size += anFloat2(x, y);
	}

	void IncreaseRotation(float rot)
	{
		Rotation += rot;
	}

	anTransformComponent() = default;
	anTransformComponent(const anTransformComponent&) = default;

	anMatrix4 GetTransformationMatrix() const
	{
		return glm::translate(anMatrix4(1.0f), { Position.x, Position.y, 0.0f })
			* glm::rotate(anMatrix4(1.0f), glm::radians(Rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(anMatrix4(1.0f), { Size.x, Size.y, 1.0f });
	}

	static anString GetComponentName() { return "TransformComponent"; }
	static void RegisterLuaAPI(sol::state& state)
	{
		auto transform = state.new_usertype<anTransformComponent>(
			"anTransform",
			"position", &anTransformComponent::Position,
			"size", &anTransformComponent::Size,
			"rotation", &anTransformComponent::Rotation,
			"increasePosition", &anTransformComponent::IncreasePosition,
			"increaseSize", &anTransformComponent::IncreaseSize,
			"increaseRotation", &anTransformComponent::IncreaseRotation
		);
	}
};

struct anSpriteRendererComponent
{
	anTexture* Texture;
	anColor Color = { 255, 255, 255 };
	int LayerNumber = 0;

	anSpriteRendererComponent() = default;
	anSpriteRendererComponent(const anSpriteRendererComponent&) = default;
	anSpriteRendererComponent(const anColor& color)
		: Color(color) {}

	static anString GetComponentName() { return "SpriteRendererComponent"; }

	static void RegisterLuaAPI(sol::state& state)
	{
		auto spriteRenderer = state.new_usertype<anSpriteRendererComponent>(
			"anSpriteRenderer",
			"color", &anSpriteRendererComponent::Color,
			"layerNumber", &anSpriteRendererComponent::LayerNumber
		);
	}
};

struct anCameraComponent
{
	anCamera2D Camera;
	bool Current = true;

	anCameraComponent() = default;
	anCameraComponent(const anCameraComponent&) = default;

	static anString GetComponentName() { return "CameraComponent"; }
};

struct anLuaScriptComponent
{
	anLuaScript* Script;

	anLuaScriptComponent() = default;
	anLuaScriptComponent(const anLuaScriptComponent&) = default;

	static anString GetComponentName() { return "LuaScriptComponent"; }
};

namespace anRigidbodyType
{
	enum : anUInt32
	{
		Static,
		Dynamic,
		Kinematic
	};
}

struct anRigidbodyComponent
{
	anUInt32 Type = anRigidbodyType::Static;
	bool FixedRotation = false;

	void* Body = nullptr;

	anRigidbodyComponent() = default;
	anRigidbodyComponent(const anRigidbodyComponent&) = default;

	static anString GetComponentName() { return "RigidbodyComponent"; }

	void ApplyLinearImpulse(const anFloat2& impulse, const anFloat2& point, bool wake);
	void ApplyLinearImpulseToCenter(const anFloat2& impulse, bool wake);
	anFloat2 GetLinearVelocity() const;
	anUInt32 GetType() const;
	void SetType(anUInt32 type);
	float GetMass() const;

	static void RegisterLuaAPI(sol::state& state)
	{
		auto rigidbody = state.new_usertype<anRigidbodyComponent>(
			"anRigidbody",
			"applyLinearImpulse", &anRigidbodyComponent::ApplyLinearImpulse,
			"applyLinearImpulseToCenter", &anRigidbodyComponent::ApplyLinearImpulseToCenter,
			"getLinearVelocity", &anRigidbodyComponent::GetLinearVelocity,
			"getType", &anRigidbodyComponent::GetType,
			"setType", &anRigidbodyComponent::SetType,
			"getMass", &anRigidbodyComponent::GetMass
		);
	}
};

struct anBoxColliderComponent
{
	anFloat2 Offset = { 0.0f, 0.0f };
	anFloat2 Size = { 0.5f, 0.5f };

	float Density = 1.0f;
	float Friction = 0.5f;
	float Restitution = 0.0f;
	float RestitutionThreshold = 0.5f;

	void* Fixture = nullptr;

	anBoxColliderComponent() = default;
	anBoxColliderComponent(const anBoxColliderComponent&) = default;

	static anString GetComponentName() { return "BoxColliderComponent"; }
};

template<typename... Component>
struct anComponentGroup
{
};

using anAllComponents = anComponentGroup<anTransformComponent, anSpriteRendererComponent,
	anLuaScriptComponent, anCameraComponent, anTagComponent, anUUIDComponent, anBoxColliderComponent,
	anRigidbodyComponent>;

#endif
