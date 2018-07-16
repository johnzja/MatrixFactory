#include "stdafx.h"
#include "Output.h"
#include <Windows.h>
using namespace std;
extern char Edition[];

void cPrintf(char* str, WORD color, bool Enter)
{
	WORD  colorOld;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle, &csbi);
	colorOld = csbi.wAttributes;
	SetConsoleTextAttribute(handle, color);
	printf(str);
	if (Enter)printf("\n");
	SetConsoleTextAttribute(handle, colorOld);
}

void init()
{
	WORD  colorOld;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle, &csbi);
	colorOld = csbi.wAttributes;
	SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);

	cout << "*********************" << "Matrix Factory" << "*********************" << endl;
	cout << "Matrix Factory v" << Edition << endl;
	cout << "Type help to get assistance." << endl;
	SetConsoleTextAttribute(handle, colorOld);

	return;
}

void isolate()
{
	cPrintf(ISOLATOR, FOREGROUND_INTENSITY | FOREGROUND_GREEN, true);

}

