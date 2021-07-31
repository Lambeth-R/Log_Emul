#pragma once
#ifndef _C_PIPE_H
#define _C_PIPE_H
#include <future>
#include <list>
#include <string>
#include <Windows.h>


struct msg {
	DWORD order;
	bool displayed;
	std::string message;
};

enum COMMANDS {
	Cnone = 0,
	Cloggin = 10,
	Cemul
};
//to do shit naming
class cPipe
{
public:
	void cmd_talker();
	void thread_log();
	void set_action(COMMANDS act);
	void clear_log();
	static cPipe* GetInstance();
	cPipe(cPipe& other) = delete;
	void operator=(const cPipe&) = delete;
	std::list<msg>* prog_log; // Connect log 
	std::list<msg>* recieved_messages;
private:
	std::future<void>* log_thread = nullptr;
	std::mutex lock;
	int thread_close;
	COMMANDS action;
	HANDLE pipe_log;
	HANDLE pipe_cmd = nullptr;
	HANDLE pipe_emul;
protected:
	static cPipe* cpipe;
	cPipe();
	~cPipe();
};


#endif

