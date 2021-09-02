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
			std::stringstream fname, hfStr, lbStr, ntrStr, lorStr, lolStr;
			hfStr << hFile;
			lbStr << std::string((char*)lpBuffer, (DWORD)*lpNumberOfBytesRead);
			ntrStr << nNumberOfBytesToRead;
			lorStr << (DWORD)*lpNumberOfBytesRead;
			lolStr << lpOverlapped;
			fname << "ReadFile";
			lsLog->push_back(&fname);
			lsLog->push_back(&hfStr);
			lsLog->push_back(&lbStr);
			lsLog->push_back(&ntrStr);
			lsLog->push_back(&lorStr);
			lsLog->push_back(&lolStr);
			std::string res = Custom_Create(*lsLog);
			Pipe* pipe = Pipe::GetInstance(pipename[1], PIPE_CREATE | PIPE_SEND);
			pipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
			pipe->PutSingleMessage(res);
		}
	}
	else {
	//if (emulater->CurrentState == COMMANDS::Cemul) {
	//	auto it = emulater->Messages.begin();
	//	std::advance(it, emulater->dCurrMess);
	//	std::list<std::stringstream*>* restored = new std::list<std::stringstream*>;
	//	if (Custom_Restore(it->message, restored)) {
	//		auto itR = restored->begin();
	//		itR++;
	//		std::string slpBuffer = itR._Ptr->_Myval->str();
	//		itR++;
	//		*itR._Ptr->_Myval >> nNumberOfBytesToRead;
	//		itR++;
	//		*itR._Ptr->_Myval >> *lpNumberOfBytesRead;
	//		if (itR._Ptr->_Myval->str().find("000000000000000") == 0)
	//			lpOverlapped;
	//		retValue = true;
	//	}
	//	else
			//Case we failed
			retValue = TrueReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);		
	}
	return retValue;
}