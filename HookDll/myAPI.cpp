#include <string>
#include <sstream>
#include "myAPI.h"
#include "../common/Pipe.h"
#include "Help.h"

HANDLE(WINAPI* TrueCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = &CreateFileW;
BOOL(WINAPI* TrueReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED) = &ReadFile;
BOOL(WINAPI* TrueWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED) = &WriteFile;
BOOL(WINAPI* TrueCloseHandle)(HANDLE) = &CloseHandle;

std::list<msg>* Create_Messages(char* text, long long length)
{
	std::list<msg>* ResList = new std::list<msg>;
	do
	{
		std::string single_msg;
		single_msg.append("22");
		single_msg.append("log>");
		std::stringstream stream1;
		int div = length > SINGLE_MSG_SIZE ? SINGLE_MSG_SIZE - single_msg.length() - 6 : length;
		stream1 << std::hex << div;
		char _size[4];
		memset(_size, 48, 4 * sizeof(char));
		int off = 0;
		if (div < 16) off = 3;
		else if (div < 256) off = 2;
		else if (div < 4096) off = 1;
		memcpy(&_size[off], stream1.str().c_str(), sizeof(char) * (4 - off));
		single_msg.append(_size,4);
		single_msg.append("<");
		single_msg.append(text, div);
		memset(text, 0, sizeof(char) * div);
		memcpy(text, &text[div], sizeof(char) * (length - div));
		length -= div;
		ResList->push_back({(DWORD)ResList->size(),false,false,single_msg });
	} while (length > 0);
	return ResList;
}

__declspec(dllexport) BOOL WINAPI CustomReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped) {
	DWORD dwWritten, dwRead;
	bool retValue = TrueReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	if (CurrentState == COMMANDS::Cloggin) {
		Sleep(10);
		std::string init_msg = "22log>0003<ini";
		char msg[256];
		std::string out;
		out += "ReadFile(";
		sprintf(msg, "%p", hFile);
		out += msg;
		out += "|||";
		out += std::string((char*)lpBuffer, *lpNumberOfBytesRead);
		out += "|||";
		out += std::to_string(nNumberOfBytesToRead);
		out += "|||";
		out += std::to_string(*lpNumberOfBytesRead);
		out += "|||";
		out += "NULL";
		out += ")";
		Pipe* pipe = Pipe::GetInstance(pipename[1], PIPE_CREATE | PIPE_SEND);
		pipe->PutMessages(*Create_Messages((char*)out.c_str(), out.size()));
	}
	return retValue;
}