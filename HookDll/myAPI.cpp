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

__declspec(dllexport) HANDLE WINAPI CustomCreateFileW(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
)
{
	HANDLE result = TrueCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	Emulater* emulater = Emulater::GetInstance();
	if (emulater->CurrentState == COMMANDS::Cloggin) {
		if (result)
		{
			std::list<std::stringstream*>* lsLog = new std::list<std::stringstream*>;
			std::stringstream sfname, sHndl, slpfname, sdDA, sSM, slpSA, dCD, dFAA, shTF;
			sfname << "CreateFileW";
			slpfname << ws2s(lpFileName);
			sHndl << result;
			sdDA << dwDesiredAccess;
			sSM << dwShareMode;
			slpSA << lpSecurityAttributes;
			dCD << dwCreationDisposition;
			dFAA << dwFlagsAndAttributes;
			shTF << hTemplateFile;
			lsLog->push_back(&sfname);
			lsLog->push_back(&sHndl);
			lsLog->push_back(&slpfname);
			lsLog->push_back(&sdDA);
			lsLog->push_back(&sSM);
			lsLog->push_back(&slpSA);
			lsLog->push_back(&dCD);
			lsLog->push_back(&dFAA);
			lsLog->push_back(&shTF);
			std::string res = Custom_Create(*lsLog);
			Pipe* pipe = Pipe::GetInstance(pipename[1], PIPE_CREATE | PIPE_SEND);
			pipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
			pipe->PutSingleMessage(res);
		}
	}
	return result;
}

__declspec(dllexport) BOOL WINAPI CustomReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
)
{
	DWORD dwWritten, dwRead;
	bool retValue = false;
	Emulater* emulater = Emulater::GetInstance();
	if (emulater->CurrentState == COMMANDS::Cloggin) {
		retValue = TrueReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		if (retValue)
		{
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
		if (emulater->CurrentState == COMMANDS::Cemul) {
			auto it = emulater->messages.find("ReadFile")->second->begin();
			// Lack of data
			while (it->displayed == true) {
				it++;
				if (it == emulater->messages.find("ReadFile")->second->end())
					return TrueReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
			}
			std::list<std::stringstream*>* restored = new std::list<std::stringstream*>;
			int* offset = new int;
			*offset = 0;
			if (ParseSingle(it->message, restored, offset)) {
				auto itR = restored->begin();
				itR++;
				itR++;
				std::string slpBuffer = itR._Ptr->_Myval->str();
				itR++;
				*itR._Ptr->_Myval >> nNumberOfBytesToRead;
				itR++;
				*itR._Ptr->_Myval >> *lpNumberOfBytesRead;
				itR++;
				if (itR._Ptr->_Myval->str().find("000000000000000") == 0)
					lpOverlapped;
				memcpy(lpBuffer, slpBuffer.c_str(), *lpNumberOfBytesRead);
				it->displayed = true;
				retValue = true;
			}
			else
				//Case we failed
				retValue = TrueReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		}
	}
	return retValue;
}

__declspec(dllexport) BOOL WINAPI CustomWriteFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
)
{
	DWORD dwWritten, dwRead;
	bool retValue = false;
	Emulater* emulater = Emulater::GetInstance();
	if (emulater->CurrentState == COMMANDS::Cloggin) {
		retValue = TrueWriteFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		if (retValue)
		{
			std::list<std::stringstream*>* lsLog = new std::list<std::stringstream*>;
			std::stringstream fname, hfStr, lbStr, ntrStr, lorStr, lolStr;
			hfStr << hFile;
			lbStr << std::string((char*)lpBuffer, (DWORD)*lpNumberOfBytesRead);
			ntrStr << nNumberOfBytesToRead;
			lorStr << (DWORD)*lpNumberOfBytesRead;
			lolStr << lpOverlapped;
			fname << "WriteFile";
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
		if (emulater->CurrentState == COMMANDS::Cemul) {
			auto it = emulater->messages.find("WriteFile")->second->begin();
			// Lack of data
			if (emulater->messages.find("WriteFile")->second->size() > 0) {
				while (it->displayed == true) {
					it++;
					if (it == emulater->messages.find("WriteFile")->second->end())
						return TrueWriteFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
				}
			}
			else {
				return TrueWriteFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
			}
			std::list<std::stringstream*>* restored = new std::list<std::stringstream*>;
			int* offset = new int;
			*offset = 0;
			if (ParseSingle(it->message, restored, offset)) {
				auto itR = restored->begin();
				itR++;
				itR++;
				std::string slpBuffer = itR._Ptr->_Myval->str();
				itR++;
				*itR._Ptr->_Myval >> nNumberOfBytesToRead;
				itR++;
				*itR._Ptr->_Myval >> *lpNumberOfBytesRead;
				itR++;
				if (itR._Ptr->_Myval->str().find("000000000000000") == 0)
					lpOverlapped;
				memcpy(lpBuffer, slpBuffer.c_str(), *lpNumberOfBytesRead);
				it->displayed = true;
				retValue = TrueWriteFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
			}
			else
				//Case we failed
				retValue = TrueWriteFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		}
	}
	return retValue;
}

__declspec(dllexport) BOOL WINAPI CustomCloseHandle(HANDLE hObject)
{
	Emulater* emulater = Emulater::GetInstance();
	if (emulater->CurrentState == COMMANDS::Cloggin) {
		std::list<std::stringstream*>* lsLog = new std::list<std::stringstream*>;
		std::stringstream sfname, ssHnd;
		sfname << "CloseHandle";
		ssHnd << hObject;
		lsLog->push_back(&sfname);
		lsLog->push_back(&ssHnd);
		std::string res = Custom_Create(*lsLog);
		Pipe* pipe = Pipe::GetInstance(pipename[1], PIPE_CREATE | PIPE_SEND);
		pipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
		pipe->PutSingleMessage(res);
	}
	return TrueCloseHandle(hObject);
}