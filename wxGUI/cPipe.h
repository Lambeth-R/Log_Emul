#pragma once
#ifndef _C_PIPE_H
#define _C_PIPE_H
#include <future>
#include <list>
#include <string>
#include <Windows.h>

// Single recieved message
struct msg {
	DWORD order;
	bool displayed;
	bool checkbox; // For emulate purposes (select if need to emulate)
	std::string message;
};
// Enumerate of command types
enum class COMMANDS {
	Cnone = 0,
	Cloggin = 10,
	Cemul
};

class cPipe
{
public:
	//Send work mode to dll
	void SetCommand();
	// Function that listens pipe data
	void ThreadLog();
	//Set action Loggin / Emulate
	void SetAction(COMMANDS act);
	// Delete all this programm log messages
	void ClearLog();
	// Send all data to dll so it can do it`s work
	void Emulate();
	// Pushes all data to Eml pipe
	void ToEmlPipe(std::list<std::string>* packeted);
	// For sync purposes and displaying info
	bool SetLock();
	// For sync purposes and displaying info
	void UnLock();
	// Div string to list of str by 1024 each
	std::list<std::string> Create_Message(char* text, long long length);
	// Singletone
	static cPipe* GetInstance();
	cPipe(cPipe& other) = delete;
	void operator=(const cPipe&) = delete;
	// Connect log 
	std::list<msg>* LogMessages; 
	// Listined data
	std::list<msg>* ListenedMessages; 
private:
	// Async thread handle
	std::future<void>* hThreadLog = nullptr;
	// Data mutex
	std::mutex DataMutex;
	// Close other threads > 0
	int bCloseThread;
	// Current working mode
	COMMANDS action;
	// Pipe handles
	HANDLE hPipeLog = nullptr;
	HANDLE hPipeCmd = nullptr;
	HANDLE hPipeEml = nullptr;
	// Set of mutex for pipes (for close purposes)
	std::mutex CmdPipeMutex;
	std::mutex LogPipeMutex;
	std::mutex EmlPipeMutex;
	// Subprocess process info
	PROCESS_INFORMATION SubProcInfo;
protected:
	// Singletone
	static cPipe* cpipe;
	cPipe();
	~cPipe();
};

#endif

