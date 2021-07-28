#pragma once
#ifndef _MY_API_H
#define _MY_API_H

#include <Windows.h>


__declspec(dllexport) BOOL WINAPI Fake_ReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped);


extern HANDLE(WINAPI* True_CreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
extern BOOL(WINAPI* True_ReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
extern BOOL(WINAPI* True_WriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
extern BOOL(WINAPI* True_CloseHandle)(HANDLE);


#endif 
