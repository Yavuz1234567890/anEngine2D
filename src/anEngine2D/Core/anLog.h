#ifndef AN_LOG_H_
#define AN_LOG_H_

#include "anTypes.h"

typedef anFunction<void(const anString&)> anLogCallback;

struct anLogContainer
{
	anLogCallback Info;
	anLogCallback Error;
	anLogCallback Warning;
};

void anSetDefaultEditorLogCallback(const anLogContainer& container);
void anSetDefaultUserLogCallback(const anLogContainer& container);
void anSetEditorLogCallback(const anLogContainer& container);
void anSetEditorUserLogCallback(const anLogContainer& container);
void anEditorInfo(const anString& msg);
void anEditorError(const anString& msg);
void anEditorWarning(const anString& msg);
void anUserInfo(const anString& msg);
void anUserError(const anString& msg);
void anUserWarning(const anString& msg);

#endif
