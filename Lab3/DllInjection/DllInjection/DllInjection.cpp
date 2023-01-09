#include <iostream>
#include <Windows.h>

using namespace std;

char INITIAL_STR_IN_MEMORY[] = "Hello";
char REPLACING_STR[] = "olleH";

#define DLL_PATH "D:\\!Studing\\3 Course\\5 Sem\\OSaSP\\Laba_3_DLL\\ReplaceString\\x64\\Debug\\ReplaceString.dll"

struct InjectParams {
	char srcStr[10];
	char destStr[10];
};

int GetPID(int*);
int GetProc(HANDLE*, DWORD);
void InjectDll(DWORD);

int main() {
	int PID = -1;
	GetPID(&PID);
	InjectDll(PID);

	return 0;
}

int GetPID(int* PID) {
	cout << "Please enter the PID to your proccess:\n";
	cin >> *PID;
	return 0;
}

int GetProc(HANDLE* handleProc, DWORD PID) {
	*handleProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	if (*handleProc == NULL) {
		cout << "Unable to open process." << endl;
		return -1;
	}
	else {
		cout << "Process opened." << endl;
		return 0;
	}
}

void InjectDll(DWORD PID) {

	HANDLE handleToProc;
	LPVOID loadLibAddr;
	LPVOID baseAddr;
	HANDLE remThread;

	cout << "Start injection." << endl;
	
	if (GetProc(&handleToProc, PID) < 0) {
		cout << "Failed to open process." << endl;
		exit(-1);
	}
	
	LPVOID loadLibraryPointer = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	if (loadLibraryPointer == NULL) {
		cout << "Can't get pointer on LoadLibraryA." << endl;
		exit(-1);
	}

	LPVOID memoryForPath = VirtualAllocEx(handleToProc, NULL, strlen(DLL_PATH), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (memoryForPath == NULL) {
		cout << "Can't alloc for DLL path." << endl;
		exit(-1);
	}
	
	if (WriteProcessMemory(handleToProc, memoryForPath, DLL_PATH, strlen(DLL_PATH) + 1, NULL) == 0) {
		cout << "Can't write DLL path in memory." << endl;
		exit(-1);
	}
	
	HANDLE thread = CreateRemoteThread(handleToProc, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryPointer, memoryForPath, 0, NULL);
	if (thread == NULL) {
		cout << "Can't create thread for DLL injection." << endl;
		exit(-1);
	}
	
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);

	LPVOID replaceFuncPointer = (LPVOID)GetProcAddress(LoadLibraryA(DLL_PATH), "CallFunc");
	if (replaceFuncPointer == NULL) {
		cout << "Can't get address of replace string function in injected DLL." << endl;
		exit(-1);
	}

	InjectParams params = {
		"Hello",
		"SgXXG.23"
	};

	// Alloc memory for parameters
	LPVOID allocPwnParamPtr = VirtualAllocEx(handleToProc, NULL, sizeof(params), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (allocPwnParamPtr == NULL) {
		cout << "Can't allocate memory in process for replace string function." << endl;
		exit(-1);
	}

	// Load parameters
	if (!WriteProcessMemory(handleToProc, allocPwnParamPtr, &params, sizeof(params), NULL)) {
		cout << "Can't write function parameters to memory." << endl;
		exit(-1);
	}

	thread = CreateRemoteThread(handleToProc, NULL, 0, (LPTHREAD_START_ROUTINE)replaceFuncPointer, allocPwnParamPtr, 0, NULL);

	if (thread == NULL) {
		cout <<  "Can't create thread for replace string function." << endl;
		exit(-1);
	}

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(thread);
	CloseHandle(handleToProc);
}
