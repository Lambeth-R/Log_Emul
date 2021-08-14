#ifndef COMMON_H
#define COMMON_H
#pragma once
#include <Windows.h>
#include <string>

struct msg {
	DWORD order;
	bool displayed;
	bool checkbox; // For emulate purposes (select if need to emulate)
	std::string message;
};

extern std::wstring pipename[3];
#endif 


