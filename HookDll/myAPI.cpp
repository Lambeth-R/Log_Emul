#include <string>
#include <sstream>
#include "myAPI.h"
#include "Emulater.h"
#include "../common/Pipe.h"
#include "../common/common.h"

HANDLE(WINAPI* TrueCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = &CreateFileW;
BOOL(WINAPI* TrueReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED) = &ReadFile;
BOOL(WINAPI* TrueWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED) = &WriteFile;
BOOL(WINAPI* TrueCloseHandle)(HANDLE) = &CloseHandle;

__declspec(dllexport) BOOL WINAPI CustomReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped) {
	DWORD dwWritten, dwRead;	
	bool retValue = false;
	Emulater* emulater = Emulater::GetInstance();
	if (emulater->CurrentState == COMMANDS::Cloggin) {
		retValue = TrueReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
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
		pipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
		//pipe->PutMessages(*Div_Messages((char*)out.c_str(), out.size()));
		pipe->PutSingleMessage(out);
	}
	else
	if (emulater->CurrentState == COMMANDS::Cemul) {
		Sleep(100);
	}
	return retValue;
}