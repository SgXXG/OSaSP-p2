// ReplaceString.h - Contains declarations of math functions
#pragma once

#ifdef REPLACESTRING_EXPORTS // Новый шаблон проекта для проекта DLL добавляется в определенные макросы препроцессора
#define REPLACESTRING_API __declspec(dllexport) // Этот модификатор предписывает компилятору и 
#else   // компоновщику экспортировать функцию или переменную из библиотеки DLL для использования другими приложениями
#define REPLACESTRING_API __declspec(dllimport)
#endif


extern "C" REPLACESTRING_API void ReplaceString(const char* data, const char* replacement);