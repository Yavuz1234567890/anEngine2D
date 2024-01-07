#include "anFileSystem.h"

#include <windows.h>

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
