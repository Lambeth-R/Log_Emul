#include "dllcPipe.h"
#include "myAPI.h"
#include "Help.h"
#include "detours.h"
#include <iostream>
#include <sstream>

dllcPipe::dllcPipe()
{
	action = COMMANDS::Cnone;
}

dllcPipe::~dllcPipe()
{
}

void dllcPipe::Create_Message(char* text, long long length)
{
	char hexString[5] = { '0' };
	char mbuf[10] = { '0' };
	do
	{
		std::string single_msg;		
		single_msg.append("22");
		single_msg.append("log");
		int div = length > SINGLE_MSG_SIZE ? SINGLE_MSG_SIZE - single_msg.length() - 6 : length;
		memset(hexString, '0', sizeof(char) * 4);
		_itoa(div, hexString, 16);
		memset(mbuf, '0', sizeof(char) * 10);
		sprintf(mbuf, ">0%s<", hexString);
		single_msg.append(mbuf,6);
		single_msg.append(text, div);
		memset(text, 0, sizeof(char) * div);
		memcpy(text, &text[div], sizeof(char) * (length - div));
		length -= div;
		this->messagesList.push_back(single_msg);
	} while (length > 0);
	this->messagesList.push_back("22log>0006<end@#$?");
	
}

void dllcPipe::Send_Message()
{

}

void dllcPipe::threader()
{
	int* exit_code;
	std::future<void>* mThread;
	while (1) {
		//Compiler suck, so no optimisation
		exit_code = new int(-1);
		mThread = new std::future<void>(std::async(std::launch::async, &dllcPipe::hPipeCommander, this, exit_code));
		athreads.push_back(thread_exit{ *exit_code, mThread});

		COMMANDS act = action;		
		switch (act)
		{
		case COMMANDS::Cloggin:
			exit_code = new int(-1);
			mThread = new std::future<void>(std::async(std::launch::async, &dllcPipe::hPipeLogger, this, exit_code));
			athreads.push_back(thread_exit{ *exit_code, mThread });
			break;
		case COMMANDS::Cemul:
			exit_code = new int(-1);
			mThread = new std::future<void>(std::async(std::launch::async, &dllcPipe::hPipeEmulater, this, exit_code));
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

void dllcPipe::SetAction(COMMANDS a)
{
	action = a;
}

COMMANDS dllcPipe::GetAction()
{
	return action;
}

void dllcPipe::ToLogPipe()
{
	DWORD dwWritten;
	if (messagesList.size() == 0) return;
	True_WriteFile(hPipeLog, messagesList.front().c_str(), SINGLE_MSG_SIZE, &dwWritten, NULL);
	if (dwWritten == SINGLE_MSG_SIZE)
		messagesList.pop_front();
}

void dllcPipe::hPipeLogger(int* exit_code)
{
	hPipeLog = CreateNamedPipe(TEXT("\\\\.\\pipe\\log_pipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (*exit_code < 0)
	{
		while (messagesList.size() > 0)
		{
			while(!DataMutex.try_lock());
			ToLogPipe();
			DataMutex.unlock();
		}
	}
}

void dllcPipe::hPipeCommander(int* exit_code)
{
	char buffer[1024];
	std::string readed_msg;
	auto me = std::this_thread::get_id();
	DWORD dwWritten, dwRead;
	hPipeCmd = CreateNamedPipe(TEXT("\\\\.\\pipe\\cmd_pipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (*exit_code < 0) {
		//ConnectNamedPipe(hPipeCmd, NULL); //Make no sense
		while (True_ReadFile(hPipeCmd, buffer, 100, &dwRead, NULL) != TRUE);
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
			action = COMMANDS::Cloggin;
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DWORD err = DetourAttach((PVOID*)&True_ReadFile, &Custom_ReadFile);
			DetourTransactionCommit();
		}
		if (readed_msg.find("22") == 0 && readed_msg.find("cmd") == 2 && strcmp(&readed_msg.c_str()[11], "eml") == 0) {
			action = COMMANDS::Cemul;
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DWORD err = DetourAttach((PVOID*)&True_ReadFile, &Custom_ReadFile);
			DetourTransactionCommit();
		}
	}
}

void dllcPipe::hPipeEmulater(int* exit_code)
{
	DWORD dwWritten, dwRead;
	std::cout << "i`m in pipe!!";
	hPipeEml = CreateNamedPipe(TEXT("\\\\.\\pipe\\emul_pipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	char buffer[1024];
	while (*exit_code < 0) {
		std::string message;
		bool end_sequence = false;
		int m_size = 0;
		do {
			while (!EmlPipeMutex.try_lock());
			while (exit_code < 0 && ReadFile(hPipeLog, buffer, SINGLE_MSG_SIZE, &dwRead, NULL) != true);
			EmlPipeMutex.unlock();
			if (exit_code >= 0)
				goto end;
			if (!Log_parse(buffer, dwRead, &m_size))
				continue;
			if (std::string(&buffer[11]).find("end@#$?") == 0)
				end_sequence = true;
			else
				message.append(std::string(&buffer[11], m_size));
		} while (!end_sequence);
		ListenedMessages->push_back({ (DWORD)ListenedMessages->size(), false, false, message });
		if (exit_code >= 0)
			goto end;
	}
	end:
	CloseHandle(hPipeEml);
}

bool Log_parse(char* buffer, int readed, int* size)
{
	if (buffer[0] != '2' || buffer[1] != '2') return false;
	if (buffer[2] != 'l' || buffer[3] != 'o' || buffer[4] != 'g') return false;
	if (buffer[5] != '>' || buffer[10] != '<') return false;
	buffer[10] = 0;
	std::stringstream temp;
	temp << &buffer[6];
	temp >> std::hex >> *size;
	return true;
}