#include "anFileSystem.h"

#ifdef PLATFORM_WINDOWS

#include <windows.h>
#include <shlobj.h>

#endif

#include <tinyfiledialogs.h>

anUInt32 anShowMessageBox(const anString& title, const anString& message, anUInt32 dialogType, anUInt32 iconType)
{
	const char* sDialogType = "ok";
	const char* sIconType = "info";
	switch (dialogType)
	{
	case anMessageBoxDialogType::Ok:
		sDialogType = "ok";
		break;
	case anMessageBoxDialogType::OkCancel:
		sDialogType = "okcancel";
		break;
	case anMessageBoxDialogType::YesNo:
		sDialogType = "yesno";
		break;
	case anMessageBoxDialogType::YesNoCancel:
		sDialogType = "yesnocancel";
		break;
	}

	switch (iconType) 
	{
	case anMessageBoxIconType::Error:
		sIconType = "error";
		break;
	case anMessageBoxIconType::Info:
		sIconType = "info";
		break;
	case anMessageBoxIconType::Warning:
		sIconType = "warning";
		break;
	case anMessageBoxIconType::Question:
		sIconType = "question";
		break;
	}

	return (anUInt32)tinyfd_messageBox(title.c_str(), message.c_str(), sDialogType, sIconType, 0);
}

bool anShowInputBox(anString& in, const anString& title, const anString& message, const anString& defaultInput)
{
	const char* str = tinyfd_inputBox(title.c_str(), message.c_str(), defaultInput.c_str());
	if (str == NULL)
		return false;

	in = str;
	return true;
}

bool anSaveFileDialog(anString& in, const anString& title, const anVector<const char*>& exts, const anString& extDesc, const anString& defaultPathAndFile)
{
	const char* str = tinyfd_saveFileDialog(title.c_str(), defaultPathAndFile.c_str(), 0, exts.data(), extDesc.c_str());
	if (str == NULL)
		return false;

	in = str;
	return true;
}

bool anOpenFileDialog(anString& in, const anString& title, const anVector<const char*>& exts, const anString& extDesc, const anString& defaultPathAndFile)
{
	const char* str = tinyfd_openFileDialog(title.c_str(), defaultPathAndFile.c_str(), 0, exts.data(), extDesc.c_str(), 0);
	if (str == NULL)
		return false;

	in = str;
	return true;
}

bool anSelectFolderDialog(anString& in, const anString& title, const anString& defPath)
{
	const char* str = tinyfd_selectFolderDialog(title.c_str(), defPath.c_str());
	if (str == NULL)
		return false;

	in = str;
	return true;
}

void anSetCurrentFolder(const anString& dir)
{
#ifdef PLATFORM_WINDOWS
	SetCurrentDirectoryA(dir.c_str());
#endif
}

void anShellExecuteOpen(const anString& location)
{
#ifdef PLATFORM_WINDOWS
	ShellExecuteA(NULL, "open", location.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#endif
}

void anShellExecute(const anString& location, const anString& parameters, bool show)
{
#ifdef PLATFORM_WINDOWS
	ShellExecuteA(NULL, "open", location.c_str(), parameters.c_str(), NULL, show ? SW_SHOWDEFAULT : SW_HIDE);
#endif
}

void anShellExecuteCmd(const anString& cmd, bool show)
{
#ifdef PLATFORM_WINDOWS
	ShellExecuteA(NULL, "open", "cmd.exe", cmd.c_str(), NULL, show ? SW_SHOWDEFAULT : SW_HIDE);
#endif
}

anFileSystem::path anGetFolderPath(anUInt32 id)
{
	anFileSystem::path res;

#ifdef PLATFORM_WINDOWS

	KNOWNFOLDERID wFolderID{};
	switch (id)
	{
	case anFolderID::AppDataLocal:
		wFolderID = FOLDERID_LocalAppData;
		break;
	case anFolderID::AppDataRoaming:
		wFolderID = FOLDERID_RoamingAppData;
		break;
	case anFolderID::ProgramFiles:
		wFolderID = FOLDERID_ProgramFiles;
		break;
	}

	PWSTR pathTmp;
	auto ret = SHGetKnownFolderPath(wFolderID, 0, nullptr, &pathTmp);

	if (ret != S_OK)
		CoTaskMemFree(pathTmp);

	res = pathTmp;
	CoTaskMemFree(pathTmp);
#endif

	return res;
}
