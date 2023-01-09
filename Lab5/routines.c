#include "routines.h"
#include <ntifs.h>


LPWSTR GetRegisterAction(REG_NOTIFY_CLASS registerAction)
{
	LPWSTR result;
	switch (registerAction)
	{
		case RegNtDeleteKey: result = L"RegNtDeleteKey"; break;
		case RegNtSetValueKey: result = L"RegNtSetValueKey"; break;
		case RegNtDeleteValueKey: result = L"RegNtDeleteValueKey"; break;
		case RegNtSetInformationKey: result = L"RegNtSetInformationKey"; break;
		case RegNtRenameKey: result = L"RegNtRenameKey"; break;
		case RegNtEnumerateKey: result = L"RegNtEnumerateKey"; break;
		case RegNtEnumerateValueKey: result = L"RegNtEnumerateValueKey"; break;
		case RegNtQueryKey: result = L"RegNtQueryKey"; break;
		case RegNtQueryValueKey: result = L"RegNtQueryValueKey"; break;
		case RegNtQueryMultipleValueKey: result = L"RegNtQueryMultipleValueKey"; break;
		case RegNtPreCreateKey: result = L"RegNtPreCreateKey"; break;
		case RegNtPostCreateKey: result = L"RegNtPostCreateKey"; break;
		case RegNtPreOpenKey: result = L"RegNtPreOpenKey"; break;
		case RegNtPostOpenKey: result = L"RegNtPostOpenKey"; break;
		case RegNtKeyHandleClose: result = L"RegNtKeyHandleClose"; break;
		case RegNtPostDeleteKey: result = L"RegNtPostDeleteKey"; break;
		case RegNtPostSetValueKey: result = L"RegNtPostSetValueKey"; break;
		case RegNtPostDeleteValueKey: result = L"RegNtPostDeleteValueKey"; break;
		case RegNtPostSetInformationKey: result = L"RegNtPostSetInformationKey"; break;
		case RegNtPostRenameKey: result = L"RegNtPostRenameKey"; break;
		case RegNtPostEnumerateKey: result = L"RegNtPostEnumerateKey"; break;
		case RegNtPostEnumerateValueKey: result = L"RegNtPostEnumerateValueKey"; break;
		case RegNtPostQueryKey: result = L"RegNtPostQueryKey"; break;
		case RegNtPostQueryValueKey: result = L"RegNtPostQueryValueKey"; break;
		case RegNtPostQueryMultipleValueKey: result = L"RegNtPostQueryMultipleValueKey"; break;
		case RegNtPostKeyHandleClose: result = L"RegNtPostKeyHandleClose"; break;
		case RegNtPreCreateKeyEx: result = L"RegNtPreCreateKeyEx"; break;
		case RegNtPostCreateKeyEx: result = L"RegNtPostCreateKeyEx"; break;
		case RegNtPreOpenKeyEx: result = L"RegNtPreOpenKeyEx"; break;
		case RegNtPostOpenKeyEx: result = L"RegNtPostOpenKeyEx"; break;
		case RegNtPreFlushKey: result = L"RegNtPreFlushKey"; break;
		case RegNtPostFlushKey: result = L"RegNtPostFlushKey"; break;
		case RegNtPreLoadKey: result = L"RegNtPreLoadKey"; break;
		case RegNtPostLoadKey: result = L"RegNtPostLoadKey"; break;
		case RegNtPreUnLoadKey: result = L"RegNtPreUnLoadKey"; break;
		case RegNtPostUnLoadKey: result = L"RegNtPostUnLoadKey"; break;
		case RegNtPreQueryKeySecurity: result = L"RegNtPreQueryKeySecurity"; break;
		case RegNtPostQueryKeySecurity: result = L"RegNtPostQueryKeySecurity"; break;
		case RegNtPreSetKeySecurity: result = L"RegNtPreSetKeySecurity"; break;
		case RegNtPostSetKeySecurity: result = L"RegNtPostSetKeySecurity"; break;
		case RegNtCallbackObjectContextCleanup: result = L"RegNtCallbackObjectContextCleanup"; break;
		case RegNtPreRestoreKey: result = L"RegNtPreRestoreKey"; break;
		case RegNtPostRestoreKey: result = L"RegNtPostRestoreKey"; break;
		case RegNtPreSaveKey: result = L"RegNtPreSaveKey"; break;
		case RegNtPostSaveKey: result = L"RegNtPostSaveKey"; break;
		case RegNtPreReplaceKey: result = L"RegNtPreReplaceKey"; break;
		case RegNtPostReplaceKey: result = L"RegNtPostReplaceKey"; break;
		case RegNtPreQueryKeyName: result = L"RegNtPreQueryKeyName"; break;
		case RegNtPostQueryKeyName: result = L"RegNtPostQueryKeyName"; break;
		case RegNtPreSaveMergedKey: result = L"RegNtPreSaveMergedKey"; break;
		case RegNtPostSaveMergedKey: result = L"RegNtPostSaveMergedKey"; break;

		default: result = NULL; break;
	}

	return result;
}


NTSTATUS OpenLogFile(PUNICODE_STRING fileName, PHANDLE hLogFile)
{
	IO_STATUS_BLOCK ioStatusBlock;
	OBJECT_ATTRIBUTES objAttributes = {0};
	InitializeObjectAttributes(&objAttributes, fileName, OBJ_OPENIF, NULL, NULL);

	return ZwCreateFile(
		hLogFile,
		FILE_APPEND_DATA,
		&objAttributes,
		&ioStatusBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN_IF,
		FILE_NON_DIRECTORY_FILE,
		NULL,
		0
	);
}

NTSTATUS CloseLogFile(HANDLE hLogFile)
{

	return ZwClose(hLogFile);
}

typedef NTSTATUS(*QUERY_INFO) (
	_In_      HANDLE           ProcessHandle,
	_In_      PROCESSINFOCLASS ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
	);


QUERY_INFO ZwQueryInformationProcess = NULL;

// If error -- *.Buffer == NULL
UNICODE_STRING GetProcessName(HANDLE handle)
{
	PWSTR ptrOnStr = NULL;
	ULONG length = PROCESS_NAME_LIMIT * sizeof(WCHAR);
	
	UNICODE_STRING res = {0};
	res.Buffer = NULL;
	res.Length = 0;
	res.MaximumLength = 0;
	
	if (ZwQueryInformationProcess == NULL)
	{
		UNICODE_STRING funcName;
		RtlInitUnicodeString(&funcName, L"ZwQueryInformationProcess");
		ZwQueryInformationProcess = (QUERY_INFO)MmGetSystemRoutineAddress(&funcName);

		if (ZwQueryInformationProcess == NULL)
		{
			DbgPrint("Can not find ZwQueryInformation\n");
			return res;
		}
	}

	SIZE_T lengthForZwAllocate = (SIZE_T) length;
	NTSTATUS status = ZwAllocateVirtualMemory(
		handle,
		&ptrOnStr,
		0,
		&lengthForZwAllocate,
		MEM_COMMIT,
		PAGE_READWRITE
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("Error in allocate memory\n");
		return res;
	}

	res.Buffer = ptrOnStr;
	length = (ULONG)lengthForZwAllocate;
	status = ZwQueryInformationProcess(handle,ProcessImageFileName,&res,(ULONG)length,&length);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("Error in getting image file name");
		SIZE_T regionSize = -1;
		ZwFreeVirtualMemory(handle, (PVOID)ptrOnStr, &regionSize, MEM_DECOMMIT);
		res.Buffer = NULL;
		res.Length = 0;
		res.MaximumLength = 0;
		return res;
	}

	return res;
}



