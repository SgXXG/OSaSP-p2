#include "ReplaceString.h"
#include <Windows.h>
#include <vector>
#include <iostream>

using namespace std;

void ReplaceString(const char* data, const char* replacement)
{
	// ��������� ���������������� �������� ��������. 
	// ���������������� � ��� ����������� ���������, � ��������� ����� ( HANDLE )-1, 
	// ������� ���������������� ��� ������� ���������� ��������.
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
		// �������� ���������� � ������� ������������ �������. 
		// ��� �������� � ���� ����������� � ��� ����������, 
		// ���������� ����������� � �������, ������ �������� � ������ �������� ����������.
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		// �������� ���������� � ��������� ������� � ����������� �������� ������������ ��������.
		MEMORY_BASIC_INFORMATION info;
		char* p = 0;

		// ��������� �� ����� ������� ����� ������, ��������� ��� ���������� � ��������� DLL.
		while (p < si.lpMaximumApplicationAddress)
		{
			// ��������� ���������� � ��������� ������� � ����������� �������� ������������ ���������� ��������.
			if (VirtualQueryEx(process, p, &info, sizeof(info)) == sizeof(info))
			{
				// MEM_COMMIT - ��������� ��������������� ��������, 
				// ��� ������� �������� ���������� ��������� ���� � ������, ���� � ����� �������� �� �����.
				// PAGE_READWRITE - ��������� ������ ������ ��� ������ ��� ������/������ � 
				// ��������������� ������� �������. ���� �������� �������������� ���������� ������ , 
				// ������� ��������� ��� � ��������������� ������� �������� � ��������� ���� �������.
				if (info.State == MEM_COMMIT && info.AllocationProtect == PAGE_READWRITE)
				{
					// ��������� �� ������� ����� ������� �������.
					p = (char*)info.BaseAddress;

					try {
						BYTE* lpData = (BYTE*)malloc(info.RegionSize);
						SIZE_T bytesRead;
						// process - ���������� �������� � �������, ������� �����������. 
						// p - ��������� �� ������� ����� � ��������� �������� ��� ������. 
						//     ������ ��� ���������� �����-���� �������� ������, 
						//     ������� ���������, ��� ��� ������ � ������� ������ � 
						//     ������ ���������� ������� �������� ��� ������, � ���� ��� ����������, 
						//     ������� ����������� �������.
						// lpData - ��������� �� �����, ������� �������� ���������� �� ��������� ������������ 
						//             ���������� ��������.
						// info.RegionSize - ���������� ������, ������� ���������� ��������� �� ���������� ��������.
						// &bytesRead - ��������� �� ����������, ������� �������� ���������� ������, 
						//              ���������� � ��������� �����.
						if (ReadProcessMemory(process, p, lpData, info.RegionSize, &bytesRead))
						{
							for (size_t i = 0; i < (bytesRead - len); ++i)
							{
								// ���������� ������ ����� ������ ����� ������, 
								// �� ������� ��������� data , � ������ ������ ������, 
								// �� ������� ���������  &lpData[i] , 
								// ��������� ����, ���� ��� ��� ���������, ��� ��������, 
								// �������� �� ����, �������������� ������� ��������, ���� ��� �� ���������.
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
				// ������ �������, ������������ � �������� ������, 
				// � ������� ��� �������� ����� ���������� ��������, � ������.
				p += info.RegionSize;
			}
		}
	}
}