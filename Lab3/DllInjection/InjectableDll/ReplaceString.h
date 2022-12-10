// ReplaceString.h - Contains declarations of math functions
#pragma once

#ifdef REPLACESTRING_EXPORTS // ����� ������ ������� ��� ������� DLL ����������� � ������������ ������� �������������
#define REPLACESTRING_API __declspec(dllexport) // ���� ����������� ������������ ����������� � 
#else   // ������������ �������������� ������� ��� ���������� �� ���������� DLL ��� ������������� ������� ������������
#define REPLACESTRING_API __declspec(dllimport)
#endif


extern "C" REPLACESTRING_API void ReplaceString(const char* data, const char* replacement);