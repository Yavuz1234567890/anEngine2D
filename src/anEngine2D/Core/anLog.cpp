#include "anLog.h"

static struct
{
	anLogContainer DefaultEditor;
	anLogContainer Editor;
	anLogContainer DefaultUser;
	anLogContainer EditorUser;
} sLogFunctions;

void anSetDefaultEditorLogCallback(const anLogContainer& container)
{
	sLogFunctions.DefaultEditor = container;
}

void anSetDefaultUserLogCallback(const anLogContainer& container)
{
	sLogFunctions.DefaultUser = container;
}

void anSetEditorLogCallback(const anLogContainer& container)
{
	sLogFunctions.Editor = container;
}

void anSetEditorUserLogCallback(const anLogContainer& container)
{
	sLogFunctions.EditorUser = container;
}

void anEditorInfo(const anString& msg)
{
	sLogFunctions.DefaultEditor.Info(msg);
	if (sLogFunctions.Editor.Info)
		sLogFunctions.Editor.Info(msg);
}

void anEditorError(const anString& msg)
{
	sLogFunctions.DefaultEditor.Error(msg);
	if (sLogFunctions.Editor.Error)
		sLogFunctions.Editor.Error(msg);
}

void anEditorWarning(const anString& msg)
{
	sLogFunctions.DefaultEditor.Warning(msg);
	if (sLogFunctions.Editor.Warning)
		sLogFunctions.Editor.Warning(msg);
}

void anUserInfo(const anString& msg)
{
	sLogFunctions.DefaultUser.Info(msg);
	if (sLogFunctions.EditorUser.Info)
		sLogFunctions.EditorUser.Info(msg);
}

void anUserError(const anString& msg)
{
	sLogFunctions.DefaultUser.Error(msg);
	if (sLogFunctions.EditorUser.Error)
		sLogFunctions.EditorUser.Error(msg);
}

void anUserWarning(const anString& msg)
{
	sLogFunctions.DefaultUser.Warning(msg);
	if (sLogFunctions.EditorUser.Warning)
		sLogFunctions.EditorUser.Warning(msg);
}
