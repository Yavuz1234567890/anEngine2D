#include "anProject.h"

#include <tinyxml2.h>

anProject* anProjectManager::sCurrentProject = nullptr;

void anProjectManager::SetCurrentProject(anProject* project)
{
	sCurrentProject = project;
}

anProject* anProjectManager::GetCurrentProject()
{
	return sCurrentProject;
}

void anProjectManager::LoadProject(const anString& path)
{
	tinyxml2::XMLDocument document;
	document.LoadFile(path.c_str());
	if (document.Error()) 
	{
		document.PrintError();
		return;
	}

	tinyxml2::XMLElement* root = document.RootElement();
	if (strcmp(root->Value(), "Application") == 0)
	{
		sCurrentProject = new anProject();
		for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			if (strcmp(child->Value(), "Name") == 0)
			{
				sCurrentProject->Name = child->Attribute("value");
				continue;
			}

			if (strcmp(child->Value(), "StartScene") == 0)
			{
				sCurrentProject->StartScene = child->Attribute("path");
				continue;
			}
		}
	}
}

void anProjectManager::SaveProject(const anString& path)
{
	if (!sCurrentProject)
		return;

	FILE* file;
	fopen_s(&file, path.c_str(), "w");
	if (!file)
		return;

	tinyxml2::XMLPrinter printer(file);

	printer.PushHeader(true, true);
	printer.OpenElement("Application");

	printer.OpenElement("Name");
	printer.PushAttribute("value", sCurrentProject->Name.c_str());
	printer.CloseElement();

	printer.OpenElement("StartScene");
	printer.PushAttribute("path", sCurrentProject->StartScene.c_str());
	printer.CloseElement();

	printer.CloseElement();

	tinyxml2::XMLDocument document;
	document.Print(&printer);
	if (file)
		fclose(file);
}

bool anProjectManager::IsProjectActive()
{
	return sCurrentProject != nullptr;
}
