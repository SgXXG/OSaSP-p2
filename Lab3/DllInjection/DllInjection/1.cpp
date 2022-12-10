#include "ReplaceString.h"
#include <Windows.h>
#include <vector>
#include <iostream>

using namespace std;

void ReplaceString(const char* data, const char* replacement)
{
	// Извлекает псевдодескриптор текущего процесса. 
	// Псевдодескриптор — это специальная константа, в настоящее время ( HANDLE )-1, 
	// которая интерпретируется как текущий дескриптор процесса.
	HANDLE process = GetCurrentProcess();

	size_t len = strlen(data);
	size_t replacementLength = strlen(replacement);

	/*cout << "Src " << sizeof(data) << endl;
	cout << "Replacement " << sizeof(replacement) << endl;
	cout << len << endl;
	cout << replacementLength << endl;*/
	if (len < replacementLength) {
		replacementLength = len;
	}

	if (process)
	{
		// Содержит информацию о текущей компьютерной системе. 
		// Это включает в себя архитектуру и тип процессора, 
		// количество процессоров в системе, размер страницы и другую подобную информацию.
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		// Содержит информацию о диапазоне страниц в виртуальном адресном пространстве процесса.
		MEMORY_BASIC_INFORMATION info;
		char* p = 0;

		// Указатель на самый высокий адрес памяти, доступный для приложений и библиотек DLL.
		while (p < si.lpMaximumApplicationAddress)
		{
			// Извлекает информацию о диапазоне страниц в виртуальном адресном пространстве указанного процесса.
			if (VirtualQueryEx(process, p, &info, sizeof(info)) == sizeof(info))
			{
				// MEM_COMMIT - Указывает зафиксированные страницы, 
				// для которых выделено физическое хранилище либо в памяти, либо в файле подкачки на диске.
				// PAGE_READWRITE - Разрешает доступ только для чтения или чтения/записи к 
				// зафиксированной области страниц. Если включено предотвращение выполнения данных , 
				// попытка выполнить код в зафиксированной области приведет к нарушению прав доступа.
				if (info.State == MEM_COMMIT && info.AllocationProtect == PAGE_READWRITE)
				{
					// Указатель на базовый адрес региона страниц.
					p = (char*)info.BaseAddress;

					try {
						BYTE* lpData = (BYTE*)malloc(info.RegionSize);
						SIZE_T bytesRead;
						// process - Дескриптор процесса с памятью, которая считывается. 
						// p - Указатель на базовый адрес в указанном процессе для чтения. 
						//     Прежде чем происходит какая-либо передача данных, 
						//     система проверяет, что все данные в базовом адресе и 
						//     памяти указанного размера доступны для чтения, и если они недоступны, 
						//     функция завершается ошибкой.
						// lpData - Указатель на буфер, который получает содержимое из адресного пространства 
						//             указанного процесса.
						// info.RegionSize - Количество байтов, которые необходимо прочитать из указанного процесса.
						// &bytesRead - Указатель на переменную, которая получает количество байтов, 
						//              переданных в указанный буфер.
						if (ReadProcessMemory(process, p, lpData, info.RegionSize, &bytesRead))
						{
							for (size_t i = 0; i < (bytesRead - len); ++i)
							{
								// Сравнивает первое число байтов блока памяти, 
								// на которое указывает data , с первым числом байтов, 
								// на которое указывает  &lpData[i] , 
								// возвращая ноль, если все они совпадают, или значение, 
								// отличное от нуля, представляющее большее значение, если они не совпадают.
								if (memcmp(data, &lpData[i], len) == 0)
								{
									char* replaceMemory = p + i;
									for (int j = 0; j < replacementLength; j++) {
										replaceMemory[j] = replacement[j];
									}
									replaceMemory[replacementLength] = 0;
								}
							}
						}
						free(lpData);
					}
					catch (std::bad_alloc& e) {
						cout << e.what() << endl;
					}
				}
				// Размер области, начинающейся с базового адреса, 
				// в которой все страницы имеют одинаковые атрибуты, в байтах.
				p += info.RegionSize;
			}
		}
	}
}