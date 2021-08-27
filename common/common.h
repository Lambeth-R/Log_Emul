#ifndef COMMON_H
#define COMMON_H
#pragma once
#include <Windows.h>
#include <string>
// Base struct that Pipe class uses. bool vars for Injector to console data.
struct msg {
	DWORD order;
	bool displayed;
	bool checkbox; // For emulate purposes (select if need to emulate)
	std::string message;
};
// All pipe names
extern std::wstring pipename[3];
std::list<msg>* Div_Messages(char* text, long long length, const char pref[] = "log");
#endif 


