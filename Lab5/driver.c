#include <ntifs.h>
#include <ntstrsafe.h>

#include "variables.h"
#include "routines.h"

PDEVICE_OBJECT deviceObject;

NTSTATUS DriverLogCallback(PVOID CallbackContext, PVOID Argument1, PVOID Argument2)
{

	HANDLE hProcess = NtCurrentProcess();
	PDRIVER_VARIABLES driverVars = (PDRIVER_VARIABLES)deviceObject->DeviceExtension;
	REG_NOTIFY_CLASS notifyClass = (REG_NOTIFY_CLASS)Argument1;
	UNICODE_STRING procName = GetProcessName(hProcess);

	if (procName.Buffer == NULL)
	{
		DbgPrint("Error in getting processName");
	}
	else
	{
		if (RtlEqualUnicodeString(&procName, &(driverVars->proccessName), FALSE))
		{
			LPWSTR logString = GetRegisterAction(notifyClass);
			IO_STATUS_BLOCK ioBlock;
			if (logString != NULL)
			{
				HANDLE hFile;
				NTSTATUS status = OpenLogFile(&(driverVars->logFilePath), &hFile);

				LARGE_INTEGER writeCode;
				writeCode.HighPart = -1;
				writeCode.LowPart = FILE_WRITE_TO_END_OF_FILE;

				if (NT_SUCCESS(status))
				{
					ZwWriteFile(hFile, NULL, NULL, NULL, &ioBlock, driverVars->proccessName.Buffer, driverVars->proccessName.Length, &writeCode, NULL);
					ZwWriteFile(hFile, NULL, NULL, NULL, &ioBlock, driverVars->delimiterCharacter.Buffer, driverVars->delimiterCharacter.Length, &writeCode, NULL);
					size_t length = 0;
					RtlStringCchLengthW(logString, PROCESS_NAME_LIMIT, &length);
					ZwWriteFile(hFile, NULL, NULL, NULL, &ioBlock, logString, length, &writeCode, NULL);
					ZwWriteFile(hFile, NULL, NULL, NULL, &ioBlock, driverVars->newLineCharacter.Buffer, driverVars->newLineCharacter.Length, &writeCode, NULL);
					CloseLogFile(hFile);
				}
			}

			//Free resources
			ZwFreeVirtualMemory(
				hProcess,
				(PVOID)procName.Buffer,
				(PSIZE_T) - 1,
				MEM_DECOMMIT
			);
		}
	}

	return STATUS_SUCCESS;
}

void DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	PDRIVER_VARIABLES vars = (PDRIVER_VARIABLES)DriverObject->DeviceObject->DeviceExtension;
	if (vars == NULL)
	{
		return;
	}

	if (vars->isRegisterCallbackSet)
	{
		CmUnRegisterCallback(vars->cookieForCallback);
	}

	IoDeleteDevice(DriverObject->DeviceObject);
	
}


NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{

	DriverObject->DriverUnload = DriverUnload;
	UNICODE_STRING deviceName;
	RtlInitUnicodeString(&deviceName, L"Laba5Device");
	
	deviceObject = NULL;
	NTSTATUS status = IoCreateDevice(
		DriverObject,
		sizeof(DRIVER_VARIABLES),
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&deviceObject
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("Error on creating device");
		return status;
	}

	InitializeDriverVariables(deviceObject);
	PDRIVER_VARIABLES vars = (PDRIVER_VARIABLES)deviceObject->DeviceExtension;


	UNICODE_STRING altitude;
	RtlInitUnicodeString(&altitude, L"10000");
	status = CmRegisterCallbackEx(
		DriverLogCallback,
		&altitude,
		DriverObject,
		NULL,
		&(vars->cookieForCallback),
		NULL
	);

	if (NT_SUCCESS(status))
	{
		vars->isRegisterCallbackSet = TRUE;
	}
	else
	{
		DbgPrint("Error on register callback initialization");
		vars->isRegisterCallbackSet = FALSE;
	}

	return STATUS_SUCCESS;

}

