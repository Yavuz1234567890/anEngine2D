#include "anSceneSerializer.h"
#include "anEntity.h"

#include <tinyxml2.h>

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
		for (tinyxml2::XMLElement* e = root->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
		{
			if (strcmp(e->Value(), "Entity") == 0)
			{
				anEntity entity = scene->NewEntity("");
				for (tinyxml2::XMLElement* child = e->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
				{
					if (strcmp(child->Value(), "Tag") == 0)
					{
						auto& component = entity.GetComponent<anTagComponent>();
						component.Tag = child->Attribute("tag");
						continue;
					}

					if (strcmp(child->Value(), "Transform") == 0)
					{
						auto& component = entity.GetComponent<anTransformComponent>();
						component.Position.x = child->FloatAttribute("px");
						component.Position.y = child->FloatAttribute("py");

						component.Size.x = child->FloatAttribute("sx");
						component.Size.y = child->FloatAttribute("sy");

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
						component.Color.R = (int)child->FloatAttribute("r");
						component.Color.G = (int)child->FloatAttribute("g");
						component.Color.B = (int)child->FloatAttribute("b");
						component.Color.A = (int)child->FloatAttribute("a");
						
						anString scenePath = child->Attribute("path");
						anFileSystem::path path = location / scenePath;
						component.Texture = scenePath.empty() ? nullptr : anLoadTexture(path.string());
						if (component.Texture != nullptr)
							component.Texture->SetScenePath(scenePath);

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
				}
			}
		}

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
	scene->GetRegistry().each([&](auto entityID)
		{
			anEntity entity = { entityID, scene };
			if (entity.GetHandle() == entt::null)
				return;

			printer.OpenElement("Entity");
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
				printer.PushAttribute("px", component.Position.x);
				printer.PushAttribute("py", component.Position.y);

				printer.PushAttribute("sx", component.Size.x);
				printer.PushAttribute("sy", component.Size.y);

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
				printer.PushAttribute("r", component.Color.R);
				printer.PushAttribute("g", component.Color.G);
				printer.PushAttribute("b", component.Color.B);
				printer.PushAttribute("a", component.Color.A);

				printer.PushAttribute("path", component.Texture == nullptr ? "" : component.Texture->GetScenePath().c_str());

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

			printer.CloseElement();
		});

	printer.CloseElement();

	tinyxml2::XMLDocument document;
	document.Print(&printer);
	if (file)
		fclose(file);
}
