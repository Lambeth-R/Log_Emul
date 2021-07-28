#include "fPipe.h"
#include "myAPI.h"
#include "Help.h"
#include "detours.h"
#include <iostream>

fPipe::fPipe()
{
	action = Cnone;
}

fPipe::~fPipe()
{
}
//Msg create problem
void fPipe::create_msg(char* text, int length)
{
	std::lock_guard<std::mutex> guard(lock);
	do
	{
		std::string single_msg;		
		single_msg.append("22");
		single_msg.append("log");
		int div = length > SINGLE_MSG_SIZE ? SINGLE_MSG_SIZE - single_msg.length() - 6 : length;
		char hexString[4] = { '0' };
		_itoa(div, &hexString[1], 16);
		char mbuf[1024] = { 0 };
		sprintf(mbuf, ">%s<", hexString);
		single_msg.append(mbuf,6);
		single_msg.append(text, div);
		memset(text, 0, sizeof(char) * div);
		memcpy(text, &text[div], sizeof(char) * (length - div));
		length -= div;
		this->msg_list.push_back(single_msg);
	} while (length > 0);
	this->msg_list.push_back("22log>0006<end@#$?");
}



void fPipe::send_msg()
{

}

void fPipe::threader()
{
	int* exit_code;
	std::future<void>* mThread;
	while (1) {
		//Compiler suck, so no optimisation
		exit_code = new int(-1);
		mThread = new std::future<void>(std::async(std::launch::async, &fPipe::pipe_commander, this, exit_code));
		athreads.push_back(thread_exit{ *exit_code, mThread});

		COMMANDS act = action;		
		switch (act)
		{
		case Cloggin:			
			exit_code = new int(-1);
			mThread = new std::future<void>(std::async(std::launch::async, &fPipe::pipe_logger, this, exit_code));
			athreads.push_back(thread_exit{ *exit_code, mThread });
			break;
		case Cemul:
			exit_code = new int(-1);
			mThread = new std::future<void>(std::async(std::launch::async, &fPipe::pipe_emulater, this, exit_code));
			athreads.push_back(thread_exit{ *exit_code, mThread });
			break;
		default:
			break;
		}
		while (act == action);
		for (auto& t : athreads)
			t.exit_code = 0;
		athreads.clear();
			
	}
}
void fPipe::setAction(COMMANDS a)
{
	action = a;
}
COMMANDS fPipe::getAction()
{
	return action;
}
void fPipe::ToPipe()
{
	std::lock_guard<std::mutex> guard(lock);
	DWORD dwWritten;
	True_WriteFile(pipe_log, msg_list.front().c_str(), SINGLE_MSG_SIZE, &dwWritten, NULL);
	if (dwWritten == SINGLE_MSG_SIZE)
		msg_list.pop_front();
}

void fPipe::pipe_logger(int* exit_code)
{
	pipe_log = CreateNamedPipe(TEXT("\\\\.\\pipe\\log_pipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	last_err = GetLastError();	
	while (*exit_code < 0)
	{
		while (msg_list.size() > 0)
		{
			ToPipe();
		}
	}
}

void fPipe::pipe_commander(int* exit_code)
{
	char buffer[1024];
	std::string readed_msg;
	auto me = std::this_thread::get_id();
	DWORD dwWritten, dwRead;
	pipe_cmd = CreateNamedPipe(TEXT("\\\\.\\pipe\\cmd_pipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	last_err = GetLastError();
	while (*exit_code < 0) {
		//ConnectNamedPipe(pipe_cmd, NULL); //Make no sense
		while (True_ReadFile(pipe_cmd, buffer, 100, &dwRead, NULL) != TRUE);
		if (buffer[0] != '2' && buffer[1] != '2') continue;
		if (buffer[2] != 'c'&& buffer[3] != 'm'&& buffer[4] != 'd') continue;
		if (buffer[5] != '>' && buffer[10] != '<')continue;
		int size = 0;
		for (int i = 0; i < 4; i++) {
			int temp = buffer[6 + i] > 64 ? buffer[6 + i] - 55 : buffer[6 + i] - 48;
			size += temp << 4;
		}
		size >>= 4;
		readed_msg = std::string(buffer,size);
		if (readed_msg.find("22") == 0 && readed_msg.find("cmd") == 2 && strcmp(&readed_msg.c_str()[11],"log") == 0) {
			action = Cloggin;
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DWORD err = DetourAttach((PVOID*)&True_ReadFile, &Fake_ReadFile);
			DetourTransactionCommit();
		}
		if (readed_msg.find("22") == 0 && readed_msg.find("cmd") == 2 && strcmp(&readed_msg.c_str()[11], "eml") == 0) {
			action = Cemul;
			//Detour here
		}
	}
}
void fPipe::pipe_emulater(int* exit_code)
{
	DWORD dwWritten, dwRead;
	std::cout << "i`m in pipe!!";
	pipe_emul = CreateNamedPipe(TEXT("\\\\.\\pipe\\emul_pipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	last_err = GetLastError();
	while (*exit_code < 0) {
		ConnectNamedPipe(pipe_emul, NULL);
	}
}