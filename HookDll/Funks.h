#pragma once
#include <Windows.h>
#include <vector>

extern BOOL(WINAPI* True_ReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
extern HANDLE(WINAPI* True_CreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

__declspec(dllexport) HANDLE WINAPI LMBT_CreateFileW(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
__declspec(dllexport) BOOL WINAPI LMBT_ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
__declspec(dllexport) BOOL WINAPI emul_LMBT_ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);

std::vector<char> typedef STRING;
extern HANDLE pipe_command, pipe_log;
extern DWORD packID;