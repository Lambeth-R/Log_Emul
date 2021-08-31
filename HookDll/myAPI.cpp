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

//	FunkId
//	1 - ReadFile
//	2 -	WriteFile
//
//
//

std::string Custom_Main(int FunkId,std::list<std::stringstream*> FuncStack)
{
	std::string res;
	switch (FunkId)
	{
	case 1:
	{
		res.append("ReadFile(");
		break;
	}
	case 2:
	{
		res.append("WriteFile(");
		break;
	}
	default:
		break;
	}
	for (auto it : FuncStack) {
		res.append(it->str());
		res.append("|||");
	}
	if (FuncStack.size() >= 1) {
		res.pop_back();
		res.pop_back();
		res.pop_back();
	}
	res.append(")");
	return res;
}

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
		if (retValue) {
			std::list<std::stringstream*>* lsLog = new std::list<std::stringstream*>;
			std::stringstream hfStr, lbStr, ntrStr, lorStr, lolStr;
			hfStr << hFile;
			lbStr << std::string((char*)lpBuffer, (DWORD)*lpNumberOfBytesRead);
			ntrStr << nNumberOfBytesToRead;
			lorStr << (DWORD)*lpNumberOfBytesRead;
			lolStr << lpOverlapped;
			lsLog->push_back(&hfStr);
			lsLog->push_back(&lbStr);
			lsLog->push_back(&ntrStr);
			lsLog->push_back(&lorStr);
			lsLog->push_back(&lolStr);
			std::string res = Custom_Main(1, *lsLog);
			Pipe* pipe = Pipe::GetInstance(pipename[1], PIPE_CREATE | PIPE_SEND);
			pipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
			pipe->PutSingleMessage(res);
		}
	}
	else
	if (emulater->CurrentState == COMMANDS::Cemul) {
		Sleep(100);
	}
	return retValue;
}