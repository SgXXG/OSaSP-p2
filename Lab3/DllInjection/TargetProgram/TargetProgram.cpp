#include <Windows.h>
#include <iostream>
#include <vector>
#include <tlhelp32.h>

#include "../../ReplaceString/ReplaceString/ReplaceString.h"

using namespace std;

int main()
{
	DWORD pid = GetCurrentProcessId();
	string localString = "Hello";

	cout << "PID: " << pid << endl;
	cout << "String before replace:" << localString << endl;
	cin.get();
	cout << "String after replace: " << localString << endl;
	return 0;
}