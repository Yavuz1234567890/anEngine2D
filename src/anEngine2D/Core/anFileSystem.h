#ifndef AN_FILE_SYSTEM_H_
#define AN_FILE_SYSTEM_H_

#include "anTypes.h"

#include <filesystem>

namespace anFileSystem = std::filesystem;

namespace anMessageBoxDialogType
{
	enum : anUInt32
	{
		Ok,
		OkCancel,
		YesNo,
		YesNoCancel
	};
}

namespace anMessageBoxIconType
{
	enum : anUInt32
	{
		Info,
		Warning,
		Error,
		Question
	};
}

namespace anMessageBoxSucces
{
	enum : anUInt32
	{
		CancelNo,
		OkYes,
		YesNoCancel
	};
}

namespace anFolderID
{
	enum : anUInt32
	{
		// for now
		AppDataLocal,
		AppDataRoaming,
		ProgramFiles
	};
}

anUInt32 anShowMessageBox(const anString& title, const anString& message, anUInt32 dialogType, anUInt32 iconType);
bool anShowInputBox(anString& in, const anString& title, const anString& message, const anString& defaultInput = "");
bool anSaveFileDialog(anString& in, const anString& title, const anVector<const char*>& exts, const anString& extDesc, const anString& defaultPathAndFile = "");
bool anOpenFileDialog(anString& in, const anString& title, const anVector<const char*>& exts, const anString& extDesc, const anString& defaultPathAndFile = "");
bool anSelectFolderDialog(anString& in, const anString& title, const anString& defPath = "");
void anSetCurrentFolder(const anString& dir);
void anShellExecuteOpen(const anString& location);
void anShellExecute(const anString& location, const anString& parameters, bool show = false);
void anShellExecuteCmd(const anString& cmd, bool show = false);
anFileSystem::path anGetFolderPath(anUInt32 id);

#endif
