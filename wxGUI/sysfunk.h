#pragma once
#ifndef _SYSFUNK_H_
#define _SYSFUNK_H_

#include <string>
#include <Windows.h>

// Base Func class
class sysfunk
{
public:
	std::string* base_string = nullptr;
	bool CorrectInited = false;
};

// ReadFile info class
class cReadFile : public sysfunk
{
public:
	cReadFile(std::string bs);
	~cReadFile();
private:
	bool Parse();
	std::string hFile;
	std::string lpBuffer;
	DWORD nNumberOfBytesToRead = 0;
	DWORD lpNumberOfBytesRead = 0;
	std::string lpOverlapped; //unsupported cos almost unused?
};

#endif // !_SYSFUNK_H_
