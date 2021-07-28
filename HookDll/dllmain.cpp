#define _CRT_SECURE_NO_WARNINGS	
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <stdio.h>
#include "fPipe.h"
#include "myAPI.h"
#include "detours.h"
#define SIZE 6

//Work status
int what_do_we_do = 0;
//Deattach
volatile BOOL isExternalRelease = FALSE;
volatile long releaseProgress = 0;
HINSTANCE selfModuleHandle;
std::future<void>* async_thread_1;
fPipe* pipe;

int WINAPI HookThread(HMODULE hModule)
{
	char buffer[1024];
	DWORD dwWritten;
	DWORD dwRead;
	fPipe pp;
	pipe = &pp;
	std::future<void> at = std::async(std::launch::async, &fPipe::threader, &pp);
	async_thread_1 = &at;
	//Old code
	/*
	pipe_command = CreateNamedPipe(TEXT("\\\\.\\pipe\\lmbtpipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	DWORD err = 0;
	if (pipe_command != INVALID_HANDLE_VALUE) {
		std::vector<STRING> test;
		if (ConnectNamedPipe(pipe_command, NULL))   // wait for someone to connect to the pipe
			{
				while (1)
				{
					if (True_ReadFile(pipe_command, buffer, 100, &dwRead, NULL) == FALSE) {
						reinit();
						DisconnectNamedPipe(pipe_command);
						FreeLibrary(selfModuleHandle);
						ExitThread(0);
						break;
					}
					parse_messge(PIHAY_V_VECTOR(buffer, dwRead), &init_msg);
					if (equal_strings(init_msg.char_string, (char*)"Listen", 7)) {
						//
						reinit();
						what_do_we_do = 11;
						DetourRestoreAfterWith();
						DetourTransactionBegin();
						DetourUpdateThread(GetCurrentThread());
						DetourAttach(&(PVOID&)True_ReadFile, LMBT_ReadFile);
						DetourTransactionCommit();
					}
					if (equal_strings(init_msg.char_string, (char*)"DropOld", 8)) {
						//		
						reinit();
						DetourTransactionBegin();
						DetourUpdateThread(GetCurrentThread());
						if (what_do_we_do == 11) DetourDetach(&(PVOID&)True_ReadFile, LMBT_ReadFile);
						if (what_do_we_do == 22) DetourDetach(&(PVOID&)True_ReadFile, emul_LMBT_ReadFile);
						what_do_we_do = 33;
						DetourTransactionCommit();
					}
					if (equal_strings(init_msg.char_string, (char*)"Emulate", 8)) {
						//
						reinit();
						what_do_we_do = 22;
						DetourRestoreAfterWith();
						DetourTransactionBegin();
						DetourUpdateThread(GetCurrentThread());
						DetourAttach(&(PVOID&)True_ReadFile, emul_LMBT_ReadFile);
						DetourTransactionCommit();
					}
					if (equal_strings(init_msg.char_string, (char*)"Deattach", 9)) {
						reinit();
						DisconnectNamedPipe(pipe_command);						
						break;
					}
				}
			}
	}	
	*/
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