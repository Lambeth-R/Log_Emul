#define _CRT_SECURE_NO_WARNINGS	
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <future>
#include <stdio.h>
#include "myAPI.h"
#include "detours.h"
#include "Emulater.h"

#include "../common/common.h"
#include "../common/Pipe.h"

//Work status
int exit_code = -1;
COMMANDS CurrentState = COMMANDS::Cnone;
HINSTANCE selfModuleHandle;


// Main thread, that responce on cmd_pipe action
int WINAPI HookThread(HMODULE hModule)
{
	DWORD dwWritten;
	DWORD dwRead;
	std::string readed_msg;
	Pipe* cPipe = Pipe::GetInstance(pipename[0], PIPE_CREATE | PIPE_RECIEVE);
	Sleep(1);
	cPipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
	std::list<msg> messages;
	Emulater* emulater = Emulater::GetInstance();
	emulater->Activate();
	while (exit_code < 0)
	{
		Sleep(100);
		std::list<msg> temp = cPipe->GetMessages();
		if (temp.size() > messages.size())
		{
			readed_msg = temp.back().message;
			emulater->SwitchContext(readed_msg);
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
		err = DetourAttach((PVOID*)&TrueCreateFileW, &CustomCreateFileW);
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