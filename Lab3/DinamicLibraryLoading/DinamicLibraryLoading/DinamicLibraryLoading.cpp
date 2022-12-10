#include <iostream>
#include <string>
#include "windows.h"
using namespace std;

typedef int FUNCREPLACEMENT(const char*, const char*);

int main()
{
    HINSTANCE hDLL; // Handle to DLL
    FUNCREPLACEMENT *ReplaceString; // Function pointer    
  
    // Загружает указанный модуль в адресное пространство вызывающего процесса. 
    // Указанный модуль может привести к загрузке других модулей.
    // Если функция завершается успешно, возвращаемое значение является дескриптором модуля.
    hDLL = LoadLibrary(L"D:\\!Studing\\3 Course\\5 Sem\\OSaSP\\Laba_3_DLL\\ReplaceString\\x64\\Debug\\ReplaceString.dll");
    if (NULL != hDLL)
    {
        // Если функция завершается успешно, возвращаемое значение является адресом экспортируемой функции или переменной.
        ReplaceString = (FUNCREPLACEMENT*)GetProcAddress(hDLL, "ReplaceString");
        if (NULL != ReplaceString)
        {
            string line = "SgXXG.23";

            cout << "String before replace:" << line.c_str() << endl;
            
            ReplaceString(line.c_str(), "Loh123!");

            cout << "String after replace:" << line.c_str() << endl;
        }
        else
        {
            std::cout << "Can`t find method\n";
        }
        FreeLibrary(hDLL);
    }
    else
    {
        std::cout << "Can`t find library\n";
    }
}