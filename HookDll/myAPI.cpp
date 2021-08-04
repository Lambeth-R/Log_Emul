#include <string>
#include "myAPI.h"
#include "dllcPipe.h"
#include "Help.h"

HANDLE(WINAPI* True_CreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = &CreateFileW;
BOOL(WINAPI* True_ReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED) = &ReadFile;
BOOL(WINAPI* True_WriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED) = &WriteFile;
BOOL(WINAPI* True_CloseHandle)(HANDLE) = &CloseHandle;

__declspec(dllexport) BOOL WINAPI Custom_ReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped) {
	DWORD dwWritten, dwRead;
	bool retValue = True_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	if (pipe->GetAction() == COMMANDS::Cloggin) {
		Sleep(10);
		while (!pipe->DataMutex.try_lock());
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
		pipe->Create_Message((char*)out.c_str(), out.length());
		pipe->DataMutex.unlock();
	}
	return retValue;
}