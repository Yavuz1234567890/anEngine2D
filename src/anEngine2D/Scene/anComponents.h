#ifndef AN_COMPONENTS_H_
#define AN_COMPONENTS_H_

#include "Core/anTypes.h"
#include "Math/anMatrix4.h"
#include "Device/anTexture.h"
#include "Renderer/anCamera2D.h"
#include "Core/anUUID.h"

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

	anTransformComponent() = default;
	anTransformComponent(const anTransformComponent&) = default;

	anMatrix4 GetTransformationMatrix() const
	{
		return glm::translate(anMatrix4(1.0f), { Position.x, Position.y, 0.0f })
			* glm::rotate(anMatrix4(1.0f), glm::radians(Rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(anMatrix4(1.0f), { Size.x, Size.y, 1.0f });
	}

	static anString GetComponentName() { return "TransformComponent"; }
};

struct anSpriteRendererComponent
{
	anTexture* Texture;
	anColor Color = { 255, 255, 255 };

	anSpriteRendererComponent() = default;
	anSpriteRendererComponent(const anSpriteRendererComponent&) = default;
	anSpriteRendererComponent(const anColor& color)
		: Color(color) {}

	static anString GetComponentName() { return "SpriteRendererComponent"; }
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

template<typename... Component>
struct anComponentGroup
{
};

using anAllComponents = anComponentGroup<anTransformComponent, anSpriteRendererComponent,
	anLuaScriptComponent, anCameraComponent, anTagComponent, anUUIDComponent>;

#endif
