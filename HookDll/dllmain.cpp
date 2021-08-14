#define _CRT_SECURE_NO_WARNINGS	
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <future>
#include <stdio.h>
#include "Help.h"
#include "myAPI.h"
#include "detours.h"

#include "../common/common.h"
#include "../common/Pipe.h"

//Work status
int exit_code = -1;
COMMANDS CurrentState = COMMANDS::Cnone;
HINSTANCE selfModuleHandle;

void SwitchContext(std::string message) {
	if (message.find("log") == 0) {
		CurrentState = COMMANDS::Cloggin;
		Pipe* lPipe = Pipe::GetInstance(pipename[1], PIPE_CREATE | PIPE_SEND);
	}
	if (message.find("eml") == 0) {
		CurrentState = COMMANDS::Cemul;
		Pipe* ePipe = Pipe::GetInstance(pipename[2], PIPE_CREATE | PIPE_RECIEVE);
	}
}
// Main thread, that responce on cmd_pipe action
int WINAPI HookThread(HMODULE hModule)
{
	DWORD dwWritten;
	DWORD dwRead;
	std::string readed_msg;
	Pipe* cPipe = Pipe::GetInstance(pipename[0], PIPE_CREATE | PIPE_RECIEVE);
	std::list<msg> messages;
	while (exit_code < 0)
	{
		Sleep(100);
		std::list<msg> temp = cPipe->GetMessages();
		if (temp.size() > messages.size())
		{
			readed_msg = temp.back().message;
			SwitchContext(readed_msg);
			messages.push_back(temp.back());
		}

	}
	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved
)
{
	selfModuleHandle = hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HookThread, hModule, 0, 0);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DWORD err = DetourAttach((PVOID*)&TrueReadFile, &CustomReadFile);
		DetourTransactionCommit();
		break;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}