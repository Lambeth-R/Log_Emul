#include <sstream>
#include "cPipe.h"
#include "contsts.h"

 cPipe::cPipe()
 {
	 prog_log = new std::list<msg>();
	 recieved_messages = new std::list<msg>();
	 thread_close = -1;
	 action = Cnone;
	 log_thread = new std::future<void>(std::async(std::launch::async, &cPipe::thread_log, this));
 }

 cPipe::~cPipe()
 {
	 delete prog_log;
	 delete recieved_messages;
 }

 void cPipe::cmd_talker()
{
	 char buffer[1024];
	 DWORD dwRead;
	 if (pipe_cmd == nullptr) {
		 while (!pipe_cmd || pipe_cmd == INVALID_HANDLE_VALUE) {
			 pipe_cmd = CreateFile(TEXT("\\\\.\\pipe\\cmd_pipe"),
				 GENERIC_READ | GENERIC_WRITE,
				 FILE_SHARE_READ, NULL,
				 OPEN_EXISTING,
				 FILE_ATTRIBUTE_NORMAL, NULL);
		 }		 
		 sprintf(buffer, "Pipe %X control connected!!!\0", pipe_cmd);
		 prog_log->push_back({ (DWORD)prog_log->size(), false, buffer });		 
	 }
	 DWORD err = 0;
	 std::string message;
	 switch (action) {
		 case Cnone: {
				message = "22cmd>000E<non";
				err = WriteFile(pipe_cmd,
				message.c_str(),
				message.length(),
				&dwRead,
				NULL);		 
			 break;
		 }
		 case Cloggin: {
			 message = "22cmd>000E<log";
			 err = WriteFile(pipe_cmd,
				 message.c_str(),
				 message.length(),
				 &dwRead,
				 NULL);
			 break;
		 }
		 case Cemul: {
			 message = "22cmd>000E<eml";
			 err = WriteFile(pipe_cmd,
				 message.c_str(),
				 message.length(),
				 &dwRead,
				 NULL);
			 break;
		 }
	 }
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

 void cPipe::thread_log()
 {
	 char buffer[1024];
	 DWORD dwRead;
	 DWORD err = 0;
	 pipe_log = nullptr;
	 while (!pipe_log || pipe_log == INVALID_HANDLE_VALUE) {
		 pipe_log = CreateFile(TEXT("\\\\.\\pipe\\log_pipe"),
			 GENERIC_READ | GENERIC_WRITE,
			 FILE_SHARE_READ, NULL,
			 OPEN_EXISTING,
			 FILE_ATTRIBUTE_NORMAL, NULL);
	 }
	 sprintf(buffer, "Pipe %X listen connected!!!\0", pipe_log);
	 prog_log->push_back({(DWORD)prog_log->size(),false,std::string(buffer)});
	 while (pipe_log != INVALID_HANDLE_VALUE || pipe_log != nullptr || thread_close < 0)
	 {
		 std::string message;
		 bool end_sequence = false;
		 int m_size = 0;
		 do {
			 while (ReadFile(pipe_log, buffer, SINGLE_MSG_SIZE, &dwRead, NULL) != true && thread_close < 0);
			 if (!Log_parse(buffer, dwRead, &m_size))
				 continue;
			 if (std::string(&buffer[11]).find("end@#$?") == 0)
				 end_sequence = true;
			 else
				 message.append(std::string(&buffer[11], m_size));
		 } while (!end_sequence);
		 recieved_messages->push_back({ (DWORD)recieved_messages->size(), false, message});
	 }
 }

void cPipe::set_action(COMMANDS act)
{
	this->action = act;
	cmd_talker();
}

void cPipe::clear_log()
{
	this->prog_log->clear();
}

cPipe* cPipe::GetInstance()
{
	if (cpipe == nullptr) {
		cpipe = new cPipe;
	}
	return cpipe;
}

cPipe* cPipe::cpipe = nullptr;