#ifndef ROUTINS_H
#define ROUTINS_H

#include <ntifs.h>
#include "variables.h"


#define PROCESS_NAME_LIMIT 256

LPWSTR GetRegisterAction(REG_NOTIFY_CLASS registerAction);

NTSTATUS OpenLogFile(PUNICODE_STRING fileName, PHANDLE hLogFile);

NTSTATUS CloseLogFile(HANDLE hLogFile);

UNICODE_STRING GetProcessName(HANDLE handle);

#endif