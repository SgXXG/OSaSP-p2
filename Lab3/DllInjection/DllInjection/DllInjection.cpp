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
	// Открывает существующий локальный объект процесса.
	// PROCESS_ALL_ACCESS - Доступ к объекту процесса. 
	// FALSE - Если это значение равно TRUE, процессы, созданные этим процессом, наследуют дескриптор. 
	//		   В противном случае процессы не наследуют этот дескриптор.
	// PID - Идентификатор локального процесса, который необходимо открыть.
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

	// Get process 
	if (GetProc(&handleToProc, PID) < 0) {
		cout << "Failed to open process." << endl;
		exit(-1);
	}

	// Извлекает адрес экспортированной функции (также называемой процедурой) 
	// или переменной из указанной библиотеки динамической компоновки (DLL).
	// GetModuleHandle(L"kernel32.dll") - Дескриптор модуля DLL, который содержит функцию или переменную
	// kernel32.dll — динамически подключаемая библиотека, являющаяся ядром всех версий ОС Microsoft Windows. 
	//				  Она предоставляет приложениям многие базовые API Win32, такие как управление памятью, 
	//				  операции ввода-вывода,создание процессов и потоков и функции синхронизации.
	// "LoadLibraryA" - Имя функции или переменной или порядковый номер функции. 
	//					Загружает указанный модуль в адресное пространство вызывающего процесса.
	LPVOID loadLibraryPointer = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	if (loadLibraryPointer == NULL) {
		cout << "Can't get pointer on LoadLibraryA." << endl;
		exit(-1);
	}

	// Резервирует, фиксирует или изменяет состояние области памяти в виртуальном адресном пространстве указанного процесса.
	// Функция инициализирует память, которую она выделяет, нулем.
	// handleToProc - Дескриптор процесса.
	// NULL - функция определяет, где выделить регион.
	// strlen(DLL_PATH) - Размер выделяемой области памяти в байтах.
	// MEM_COMMIT | MEM_RESERVE - зарезервировать и зафиксировать страницы за один шаг.
	// PAGE_EXECUTE_READWRITE - Разрешает выполнение, доступ только для чтения или доступ для чтения/записи к 
	//							зафиксированной области страниц.
	LPVOID memoryForPath = VirtualAllocEx(handleToProc, NULL, strlen(DLL_PATH), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (memoryForPath == NULL) {
		cout << "Can't alloc for DLL path." << endl;
		exit(-1);
	}

	// Записывает данные в область памяти в указанном процессе. 
	// Вся область для записи должна быть доступна, иначе операция завершится ошибкой.
	// handleToProc - Дескриптор изменяемой памяти процесса.
	// allocForStrings - Указатель на базовый адрес в указанном процессе, в который записываются данные.
	// DLL_PATH - Указатель на буфер, содержащий данные для записи в адресное пространство указанного процесса.
	// strlen(DLL_PATH) + 1 - Количество байтов, которые необходимо записать в указанный процесс.
	// NULL - Указатель на переменную, которая получает количество байтов, переданных в указанный процесс.
	if (WriteProcessMemory(handleToProc, memoryForPath, DLL_PATH, strlen(DLL_PATH) + 1, NULL) == 0) {
		cout << "Can't write DLL path in memory." << endl;
		exit(-1);
	}

	// Создает поток, работающий в виртуальном адресном пространстве другого процесса.
	// handleToProc - дескриптор процесса, в котором должен быть создан поток. 
	// NULL - Указатель на структуру SECURITY_ATTRIBUTES.
	// 0 - Начальный размер стека в байтах. 
	// loadLibraryPointer - Указатель на определяемую приложением функцию типа LPTHREAD_START_ROUTINE , 
	//                       которая должна выполняться потоком, и представляет начальный адрес потока в удаленном процессе. 
	// memoryForPath - Указатель на переменную, которая будет передана функции потока.
	// 0 - Поток запускается сразу после создания.
	// NULL - Указатель на переменную, которая получает идентификатор потока.
	HANDLE thread = CreateRemoteThread(handleToProc, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryPointer, memoryForPath, 0, NULL);
	if (thread == NULL) {
		cout << "Can't create thread for DLL injection." << endl;
		exit(-1);
	}

	// Ожидает, пока указанный объект не перейдет в сигнальное состояние или пока не истечет время ожидания.
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);

	LPVOID replaceFuncPointer = (LPVOID)GetProcAddress(LoadLibraryA(DLL_PATH), "CallFunc");
	if (replaceFuncPointer == NULL) {
		cout << "Can't get address of replace string function in injected DLL." << endl;
		exit(-1);
	}

	InjectParams params = {
		"Hello",
		"Irin1"
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
