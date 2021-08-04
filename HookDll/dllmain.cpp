#define _CRT_SECURE_NO_WARNINGS	
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <stdio.h>
#include "dllcPipe.h"
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
dllcPipe* pipe;

int WINAPI HookThread(HMODULE hModule)
{
	char buffer[1024];
	DWORD dwWritten;
	DWORD dwRead;
	dllcPipe pp;
	pipe = &pp;
	std::future<void> at = std::async(std::launch::async, &dllcPipe::threader, &pp);
	async_thread_1 = &at;
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