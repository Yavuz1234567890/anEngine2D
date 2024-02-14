#include "anProject.h"

#include <tinyxml2.h>

anDisplayResolution anDisplayResolution::DisplayResolutions[DISPLAY_RESOLUTIONS_SIZE];

void anDisplayResolution::Initialize()
{
	DisplayResolutions[0] = { 640, 360 };
	DisplayResolutions[1] = { 854, 480 };
	DisplayResolutions[2] = { 960, 540 };
	DisplayResolutions[3] = { 1024, 576 };
	DisplayResolutions[4] = { 1280, 720 };
	DisplayResolutions[5] = { 1366, 768 };
	DisplayResolutions[6] = { 1600, 900 };
	DisplayResolutions[7] = { 1920, 1080 };
}

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

			if (strcmp(child->Value(), "IsFullscreen") == 0)
			{
				sCurrentProject->IsFullscreen = child->BoolAttribute("fullscreen");
				continue;
			}

			if (strcmp(child->Value(), "DisplayResolution") == 0)
			{
				sCurrentProject->ResolutionID = child->IntAttribute("id");
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

	printer.OpenElement("IsFullscreen");
	printer.PushAttribute("fullscreen", sCurrentProject->IsFullscreen);
	printer.CloseElement();

	printer.OpenElement("DisplayResolution");
	printer.PushAttribute("id", sCurrentProject->ResolutionID);
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
