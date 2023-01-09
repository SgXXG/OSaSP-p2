#ifndef VARIABLES_H
#define VARIABLES_H

#include <ntifs.h>


typedef struct _DRIVER_VARIABLES
{
	BOOLEAN isRegisterCallbackSet;
	LARGE_INTEGER cookieForCallback;
	
	UNICODE_STRING logFilePath;
	UNICODE_STRING newLineCharacter;
	UNICODE_STRING proccessName;
	UNICODE_STRING delimiterCharacter;

} DRIVER_VARIABLES, * PDRIVER_VARIABLES;

void InitializeDriverVariables(PDEVICE_OBJECT deviceObject);

#endif