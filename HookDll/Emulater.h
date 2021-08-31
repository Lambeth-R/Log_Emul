#ifndef EMULATER_H
#define EMULATER_H
#pragma once

#include <list>
#include "Help.h"
#include "../common/common.h"
#include "../common/Pipe.h"

enum class COMMANDS {
	Cnone = 0,
	Cloggin = 10,
	Cemul
};

extern BOOL(WINAPI* TrueReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
extern BOOL(WINAPI* TrueWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);

class Emulater
{
public:
	COMMANDS CurrentState;
	std::list<msg> Messages;

private:
	bool active = false;
	Pipe *cPipe = nullptr, *lPipe = nullptr, *ePipe = nullptr;
	void SyncMsg();
	void wait(Pipe* Pipe, int size);
	std::mutex mux_cPipe, mux_lPipe, mux_ePipe;
	HANDLE hThread;
public:
	void SwitchContext(std::string message);
	void Activate();
	static Emulater* GetInstance();
	Emulater(Emulater& other) = delete;
	void operator=(const Emulater&) = delete;


protected:
	// Singletone
	static Emulater* emulater;
	Emulater();
	~Emulater();
};

#endif // !EMULATER_H