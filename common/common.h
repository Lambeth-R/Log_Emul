#ifndef COMMON_H
#define COMMON_H
#pragma once
#include <string>
#include <codecvt>

#include <Windows.h>
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
std::string wtochar(std::wstring string);

long long HexToInt(std::string value);
std::string IntToHex(long long value);
std::string Custom_Create(std::list<std::stringstream*> FuncStack);
bool ParseSingle(std::string orig_message, std::list<std::stringstream*>* result, int* offset);
std::list<msg>* ParseFile(std::string wholeFile);
std::string CreateLogText(std::list<msg> lMessages);
std::list<msg>* ReadFromLog(std::string name);
bool CreateLogFile(std::list<msg> lMessages, std::string* name);

std::string ws2s(const std::wstring& wstr);
std::wstring s2ws(const std::string& str);

#endif 


