#include <iostream>
#include <string>
#include "ReplaceString.h"
using namespace std;

int main()
{
	string line = "SgXXG.23";

	cout << "String before replace:" << endl;
	cout << line.c_str() << endl;
	
	ReplaceString(line.c_str(), "Loh123");

	cout << "String after replace:" << endl;
	cout << line.c_str() << endl;
}