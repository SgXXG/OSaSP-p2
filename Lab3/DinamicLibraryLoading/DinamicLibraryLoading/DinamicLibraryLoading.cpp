#include <iostream>
#include <string>
#include "windows.h"
using namespace std;

typedef int FUNCREPLACEMENT(const char*, const char*);

int main()
{
    HINSTANCE hDLL;
    FUNCREPLACEMENT *ReplaceString;   
    
    hDLL = LoadLibrary(L"D:\\!Studing\\3 Course\\5 Sem\\OSaSP\\Laba_3_DLL\\ReplaceString\\x64\\Debug\\ReplaceString.dll");
    if (NULL != hDLL)
    {
        ReplaceString = (FUNCREPLACEMENT*)GetProcAddress(hDLL, "ReplaceString");
        if (NULL != ReplaceString)
        {
            string line = "SgXXG.23";

            cout << "String before replace:" << endl;
            cout << line.c_str() << endl;
            
            ReplaceString(line.c_str(), "Privet12");

            cout << "String after replace:" << endl;
            cout << line.c_str() << endl;
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