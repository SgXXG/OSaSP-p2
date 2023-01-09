#include "variables.h"

#define LOG_LOCATION L"C:\\Laba5Driver.txt"

#define PROCESS_NAME L"regedit.exe"

void InitializeDriverVariables(PDEVICE_OBJECT deviceObject)
{
	PDRIVER_VARIABLES driverVars = (PDRIVER_VARIABLES)deviceObject->DeviceExtension;

	if (driverVars == NULL)
	{
		return;
	}

	driverVars->isRegisterCallbackSet = FALSE;
	RtlInitUnicodeString(&(driverVars->delimiterCharacter), L" : ");
	RtlInitUnicodeString(&(driverVars->logFilePath), LOG_LOCATION);
	RtlInitUnicodeString(&(driverVars->newLineCharacter), L" \n");
	RtlInitUnicodeString(&(driverVars->proccessName), PROCESS_NAME);

}