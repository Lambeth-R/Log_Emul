#pragma once
#ifndef  _F_PIPE_H_
#define _F_PIPE_H_


#include <string>
#include <list>
#include <future>
#include <iostream>
#include <mutex>
#include <Windows.h>


enum class COMMANDS {
	Cnone = 0,
	Cloggin = 10,
	Cemul
};
struct thread_exit {
	int& exit_code;
	std::future<void>* thread;
};

// Single recieved message
struct msg {
	DWORD order;
	bool displayed;
	bool checkbox; // For emulate purposes (select if need to emulate)
	std::string message;
};

class dllcPipe
{
public:
	dllcPipe();
	~dllcPipe();
	void Create_Message(char* text, long long length);
	void Send_Message();
	void SetAction(COMMANDS a);
	COMMANDS GetAction();
	void threader();
	std::mutex DataMutex;
private:
	void hPipeLogger(int* exit_code);
	void hPipeCommander(int* exit_code);
	void hPipeEmulater(int* exit_code);
	void ToLogPipe();

	std::list<msg>* ListenedMessages;
	COMMANDS action;
	HANDLE hPipeLog = nullptr;
	HANDLE hPipeCmd = nullptr;
	HANDLE hPipeEml = nullptr;
	// Set of mutex for pipes (for close purposes)
	std::mutex CmdPipeMutex;
	std::mutex LogPipeMutex;
	std::mutex EmlPipeMutex;
	std::list<std::string> messagesList;
	std::list <thread_exit> athreads;
};
// Todo put this outside class code
bool Log_parse(char* buffer, int readed, int* size);

#endif
