#include <string>
#include "myAPI.h"
#include "fPipe.h"
#include "Help.h"

HANDLE(WINAPI* True_CreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = &CreateFileW;
BOOL(WINAPI* True_ReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED) = &ReadFile;
BOOL(WINAPI* True_WriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED) = &WriteFile;
BOOL(WINAPI* True_CloseHandle)(HANDLE) = &CloseHandle;

__declspec(dllexport) BOOL WINAPI Fake_ReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped) {
	DWORD dwWritten, dwRead;
	bool retValue = True_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	if (pipe->getAction() == Cloggin) {
		Sleep(1000);
		//std::string t = "Listen_";
		//t += std::to_string(*lpNumberOfBytesRead);
		std::string init_msg = "22log>0003<ini"; //????
		char msg[256];
		std::string out;
		out += "ReadFile(";
		sprintf(msg, "%p", hFile); //Handle
		out += msg;
		out += "|||";
		out += std::string((char*)lpBuffer, *lpNumberOfBytesRead);
		//for (int i = 0; i < *lpNumberOfBytesRead; i++) //Buff
		//{
		//	out += ((char*)lpBuffer)[i];
		//}
		out += "|||";
		out += std::to_string(nNumberOfBytesToRead); //To read
		out += "|||";
		out += std::to_string(*lpNumberOfBytesRead);	//Readed
		out += "|||";
		out += "NULL";	//Null???
		out += ")";
		pipe->create_msg((char*)out.c_str(), out.length());
		//Ѕить на пакеты!
		//for (int i = 0; i < out.size(); i += 1012) {
		//	int sz_to_send = 1012;
		//	if (i + 1024 > out.size())
		//		sz_to_send = out.size() - i;
		//	std::string packet = make_msg((char*)out.substr(i, i + 1012).c_str(), sz_to_send);
		//	True_WriteFile(pipe_log,
		//		packet.data(),
		//		1024,
		//		&dwWritten,
		//		NULL);
		//}
	}
	return retValue;
}