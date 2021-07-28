#pragma once
#ifndef  _F_PIPE_H_
#define _F_PIPE_H_


#include <string>
#include <list>
#include <future>
#include <iostream>
#include <mutex>
#include <Windows.h>


enum COMMANDS {
	Cnone = 0,
	Cloggin = 10,
	Cemul
};
struct thread_exit {
	int& exit_code;
	std::future<void>* thread;
};


class fPipe
{
public:
	fPipe();
	~fPipe();
	void create_msg(char* text, int length);
	void send_msg();
	void setAction(COMMANDS a);
	COMMANDS getAction();
	void threader();
private:
	void pipe_logger(int* exit_code);
	void pipe_commander(int* exit_code);
	void pipe_emulater(int* exit_code);
	void ToPipe();
	COMMANDS action;
	HANDLE pipe_log;
	HANDLE pipe_cmd;
	HANDLE pipe_emul;
	std::list<std::string> msg_list;
	DWORD last_err;
	std::list <thread_exit> athreads;
	std::mutex lock;
};



#endif
