#include "anSceneSerializer.h"
#include "anEntity.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>

anSceneSerializer::anSceneSerializer()
{
}

anSceneSerializer::~anSceneSerializer()
{
}

static void DeserializeFloat2(const anString& fname, anFloat2& f2, tinyxml2::XMLElement* element)
{
	f2.x = element->FloatAttribute((fname + "x").c_str());
	f2.y = element->FloatAttribute((fname + "y").c_str());
}

static void SerializeFloat2(const anString& fname, const anFloat2& f2, tinyxml2::XMLPrinter& printer)
{
	printer.PushAttribute((fname + "x").c_str(), f2.x);
	printer.PushAttribute((fname + "y").c_str(), f2.y);
}

static void DeserializeColor(const anString& cname, anColor& color, tinyxml2::XMLElement* element)
{
	color.R = element->IntAttribute((cname + "r").c_str());
	color.G = element->IntAttribute((cname + "g").c_str());
	color.B = element->IntAttribute((cname + "b").c_str());
	color.A = element->IntAttribute((cname + "a").c_str());
}

static void SerializeColor(const anString& cname, const anColor& color, tinyxml2::XMLPrinter& printer)
{
	printer.PushAttribute((cname + "r").c_str(), color.R);
	printer.PushAttribute((cname + "g").c_str(), color.G);
	printer.PushAttribute((cname + "b").c_str(), color.B);
	printer.PushAttribute((cname + "a").c_str(), color.A);
}

anEntity anSceneSerializer::DeserializeEntity(const anFileSystem::path& location, tinyxml2::XMLElement* element, anScene* scene)
{
	if (strcmp(element->Value(), "Entity") == 0)
	{
		anEntity entity = scene->NewEntity("");
		for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			if (strcmp(child->Value(), "UUID") == 0)
			{
				auto& component = entity.GetComponent<anUUIDComponent>();
				component.UUID = anUUID((anUInt64)child->Attribute("uuid"));
				continue;
			}

			if (strcmp(child->Value(), "Tag") == 0)
			{
				auto& component = entity.GetComponent<anTagComponent>();
				component.Tag = child->Attribute("tag");
				continue;
			}

			if (strcmp(child->Value(), "Transform") == 0)
			{
				auto& component = entity.GetComponent<anTransformComponent>();
				DeserializeFloat2("p", component.Position, child);
				DeserializeFloat2("s", component.Size, child);

				component.Rotation = child->FloatAttribute("rot");
				continue;
			}

			if (strcmp(child->Value(), "Camera") == 0)
			{
				auto& component = entity.AddComponent<anCameraComponent>();
				component.Current = child->BoolAttribute("current");
				continue;
			}

			if (strcmp(child->Value(), "SpriteRenderer") == 0)
			{
				auto& component = entity.AddComponent<anSpriteRendererComponent>();
				DeserializeColor("color", component.Color, child);

				anString scenePath = child->Attribute("path");
				anFileSystem::path path = location / scenePath;
				component.Texture = scenePath.empty() ? nullptr : anLoadTexture(path.string());
				if (component.Texture != nullptr)
					component.Texture->SetEditorPath(scenePath);

				component.LayerNumber = child->IntAttribute("layerNumber");
				continue;
			}

			if (strcmp(child->Value(), "LuaScript") == 0)
			{
				auto& component = entity.AddComponent<anLuaScriptComponent>();

				anString scriptPath = child->Attribute("path");
				if (!scriptPath.empty())
				{
					component.Script = new anLuaScript();
					component.Script->LoadScript(location / scriptPath, scriptPath);
				}

				continue;
			}

			if (strcmp(child->Value(), "Rigidbody") == 0)
			{
				auto& component = entity.AddComponent<anRigidbodyComponent>();
				anString type = child->Attribute("type");
				if (type == "static")
					component.Type = anRigidbodyType::Static;
				else if (type == "dynamic")
					component.Type = anRigidbodyType::Dynamic;
				else if (type == "kinematic")
					component.Type = anRigidbodyType::Kinematic;

				component.FixedRotation = child->BoolAttribute("fixedRotation");

				continue;
			}

			if (strcmp(child->Value(), "BoxCollider") == 0)
			{
				auto& component = entity.AddComponent<anBoxColliderComponent>();
				component.Density = child->FloatAttribute("density");
				component.Friction = child->FloatAttribute("friction");
				DeserializeFloat2("offset", component.Offset, child);
				component.Restitution = child->FloatAttribute("restitution");
				component.RestitutionThreshold = child->FloatAttribute("restitutionThreshold");
				DeserializeFloat2("size", component.Size, child);

				continue;
			}
		}

		return entity;
	}

	return {};
}

void anSceneSerializer::SerializeEntity(const anFileSystem::path& location, const anEntity& ent, tinyxml2::XMLPrinter& printer)
{
	anEntity entity = ent;
	if (entity.GetHandle() == entt::null)
		return;

	printer.OpenElement("Entity");

	if (entity.HasComponent<anUUIDComponent>())
	{
		auto& component = entity.GetComponent<anUUIDComponent>();

		printer.OpenElement("UUID");
		printer.PushAttribute("uuid", (anUInt64)component.UUID);
		printer.CloseElement();
	}

	if (entity.HasComponent<anTagComponent>())
	{
		auto& component = entity.GetComponent<anTagComponent>();

		printer.OpenElement("Tag");
		printer.PushAttribute("tag", component.Tag.c_str());
		printer.CloseElement();
	}

	if (entity.HasComponent<anTransformComponent>())
	{
		auto& component = entity.GetComponent<anTransformComponent>();

		printer.OpenElement("Transform");
		SerializeFloat2("p", component.Position, printer);
		SerializeFloat2("s", component.Size, printer);

		printer.PushAttribute("rot", component.Rotation);

		printer.CloseElement();
	}

	if (entity.HasComponent<anCameraComponent>())
	{
		auto& component = entity.GetComponent<anCameraComponent>();

		printer.OpenElement("Camera");
		printer.PushAttribute("current", component.Current);
		printer.CloseElement();
	}

	if (entity.HasComponent<anSpriteRendererComponent>())
	{
		auto& component = entity.GetComponent<anSpriteRendererComponent>();

		printer.OpenElement("SpriteRenderer");
		SerializeColor("color", component.Color, printer);

		printer.PushAttribute("path", component.Texture == nullptr ? "" : component.Texture->GetEditorPath().c_str());

		printer.PushAttribute("layerNumber", component.LayerNumber);

		printer.CloseElement();
	}

	if (entity.HasComponent<anLuaScriptComponent>())
	{
		auto& component = entity.GetComponent<anLuaScriptComponent>();

		printer.OpenElement("LuaScript");

		anString path = component.Script ? component.Script->GetEditorPath().string() : "";
		printer.PushAttribute("path", path.c_str());

		printer.CloseElement();
	}

	if (entity.HasComponent<anRigidbodyComponent>())
	{
		auto& component = entity.GetComponent<anRigidbodyComponent>();
		printer.OpenElement("Rigidbody");

		switch (component.Type)
		{
		case anRigidbodyType::Dynamic:
			printer.PushAttribute("type", "dynamic");
			break;
		case anRigidbodyType::Static:
			printer.PushAttribute("type", "static");
			break;
		case anRigidbodyType::Kinematic:
			printer.PushAttribute("type", "kinematic");
			break;
		}

		printer.PushAttribute("fixedRotation", component.FixedRotation);
		
		printer.CloseElement();
	}

	if (entity.HasComponent<anBoxColliderComponent>())
	{
		auto& component = entity.GetComponent<anBoxColliderComponent>();

		printer.OpenElement("BoxCollider");

		printer.PushAttribute("density", component.Density);
		printer.PushAttribute("friction", component.Friction);
		SerializeFloat2("offset", component.Offset, printer);
		printer.PushAttribute("restitution", component.Restitution);
		printer.PushAttribute("restitutionThreshold", component.RestitutionThreshold);
		SerializeFloat2("size", component.Size, printer);

		printer.CloseElement();
	}

	printer.CloseElement();
}

anScene* anSceneSerializer::DeserializeScene(const anFileSystem::path& location, const anFileSystem::path& filePath)
{
	tinyxml2::XMLDocument document;
	document.LoadFile(filePath.string().c_str());
	if (document.Error()) 
	{
		document.PrintError();
		return nullptr;
	}

	tinyxml2::XMLElement* root = document.RootElement();
	if (strcmp(root->Value(), "Scene") == 0)
	{
		anScene* scene = new anScene();
		DeserializeColor("clearColor", scene->GetClearColor(), root);
		for (tinyxml2::XMLElement* e = root->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
			DeserializeEntity(location, e, scene);

		return scene;
	}

	return nullptr;
}

void anSceneSerializer::SerializeScene(const anFileSystem::path& location, anScene* scene, const anFileSystem::path& filePath)
{
	FILE* file;
	fopen_s(&file, filePath.string().c_str(), "w");
	if (!file)
		return;

	tinyxml2::XMLPrinter printer(file);

	printer.PushHeader(true, true);
	printer.OpenElement("Scene");
	SerializeColor("clearColor", scene->GetClearColor(), printer);

	scene->GetRegistry().each([&](auto entityID)
		{
			anEntity entity = { entityID, scene };
			SerializeEntity(location, entity, printer);
		});

	printer.CloseElement();

	tinyxml2::XMLDocument document;
	document.Print(&printer);
	if (file)
		fclose(file);
}
