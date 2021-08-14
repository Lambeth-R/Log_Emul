#include <sstream>
#include "cPipe.h"
#include "cMain.h"
#include "contsts.h"

 cPipe::cPipe()
 {
	 LogMessages = new std::list<msg>();
	 ListenedMessages = new std::list<msg>();
	 bCloseThread = -1;
	 action = COMMANDS::Cnone;
 }

 cPipe::~cPipe()
 {
	 delete LogMessages;
	 delete ListenedMessages;
	 if (SubProcInfo.hProcess) {
		 TerminateProcess(SubProcInfo.hProcess,0);
	 }
 }

void cPipe::SetCommand()
{
	 char buffer[1024];
	 DWORD dwRead;
	 if (hPipeCmd == nullptr) {
		 while (!hPipeCmd || hPipeCmd == INVALID_HANDLE_VALUE) {
			 hPipeCmd = CreateFile(TEXT("\\\\.\\pipe\\cmd_pipe"),
				 GENERIC_READ | GENERIC_WRITE,
				 FILE_SHARE_READ, NULL,
				 OPEN_EXISTING,
				 FILE_ATTRIBUTE_NORMAL, NULL);
		 }		 
		 sprintf(buffer, "Pipe %X control connected!!!\0", hPipeCmd);
		 LogMessages->push_back({ (DWORD)LogMessages->size(), false, false,buffer });
	 }
	 DWORD err = 0;
	 std::string message;
	 switch (action) {
	 case COMMANDS::Cnone: {
			message = "22cmd>000E<non";
			while (!CmdPipeMutex.try_lock());
			err = WriteFile(hPipeCmd,
				message.c_str(),
				message.length(),
				&dwRead,
				NULL);
			CmdPipeMutex.unlock();
			 break;
		 }
	 case COMMANDS::Cloggin: {
			 message = "22cmd>000E<log";
			 while (!CmdPipeMutex.try_lock());
			 err = WriteFile(hPipeCmd,
				 message.c_str(),
				 message.length(),
				 &dwRead,
				 NULL);
			 CmdPipeMutex.unlock();
			 bCloseThread = 1;
			 while (EmlPipeMutex.try_lock());
			 if (hPipeEml) CloseHandle(hPipeEml);
			 EmlPipeMutex.unlock();
			 Sleep(100);
			 bCloseThread = -1;
			 hThreadLog = new std::future<void>(std::async(std::launch::async, &cPipe::ThreadLog, this));
			 break;
		 }
	 case COMMANDS::Cemul: {
			 message = "22cmd>000E<eml";
			 while (!CmdPipeMutex.try_lock());
			 err = WriteFile(hPipeCmd,
				 message.c_str(),
				 message.length(),
				 &dwRead,
				 NULL);
			 CmdPipeMutex.unlock();
			 bCloseThread = 1;
			 //while (LogPipeMutex.try_lock());
			 //if (hPipeLog) CloseHandle(hPipeLog);
			 //LogPipeMutex.unlock();
			 Sleep(100);
			 bCloseThread = -1;
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

void cPipe::ThreadLog()
 {
	 char buffer[1024];
	 DWORD dwRead;
	 DWORD err = 0;
	 hPipeLog = nullptr;
	 while (!hPipeLog || hPipeLog == INVALID_HANDLE_VALUE) {
		 hPipeLog = CreateFile(TEXT("\\\\.\\pipe\\log_pipe"),
			 GENERIC_READ | GENERIC_WRITE,
			 FILE_SHARE_READ, NULL,
			 OPEN_EXISTING,
			 FILE_ATTRIBUTE_NORMAL, NULL);
	 }
	 sprintf(buffer, "Pipe %X listen connected!!!\0", hPipeLog);
	 LogMessages->push_back({ (DWORD)LogMessages->size(),false,false,std::string(buffer) });
	 while (bCloseThread < 0)
	 {
		 std::string message;
		 bool end_sequence = false;
		 int m_size = 0;
		 do {
			 while (!LogPipeMutex.try_lock());
			 while (bCloseThread < 0 && ReadFile(hPipeLog, buffer, SINGLE_MSG_SIZE, &dwRead, NULL) != true);
			 LogPipeMutex.unlock();
			 if (bCloseThread >= 0)
				 goto end;
			 if (!Log_parse(buffer, dwRead, &m_size))
				 continue;
			 if (std::string(&buffer[11]).find("end@#$?") == 0)
				 end_sequence = true;
			 else
				 message.append(std::string(&buffer[11], m_size));
		 } while (!end_sequence);
		 ListenedMessages->push_back({ (DWORD)ListenedMessages->size(), false, false,message });
		 if (bCloseThread >= 0)
			 goto end;
	 }
	 end:
	 CloseHandle(hPipeLog);
 }

void cPipe::SetAction(COMMANDS act)
{
	this->action = act;
	SetCommand();
}

void cPipe::ClearLog()
{
	while (!SetLock());
	this->LogMessages->clear();
	UnLock();
}

std::list<std::string> cPipe::Create_Message(char* text, long long length)
{
	std::list<std::string> result;
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
		single_msg.append(mbuf, 6);
		single_msg.append(text, div);
		memset(text, 0, sizeof(char) * div);
		memcpy(text, &text[div], sizeof(char) * (length - div));
		length -= div;
		result.push_back(single_msg);
	} while (length > 0);
	result.push_back("22log>0006<end@#$?");
	return result;
}

void cPipe::ToEmlPipe(std::list<std::string>* packeted)
{
	DWORD dwWritten;
	bool error;
	for (auto it = packeted->begin(); it != packeted->end(); it++) {
		dwWritten = NULL;
		error = WriteFile(hPipeEml, it->c_str(), SINGLE_MSG_SIZE, &dwWritten, NULL);
	}
}

void cPipe::Emulate()
{
	Process kill_me = cMain::GetInstance()->GetProcess();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, kill_me.pid);
	if (!hProcess)
	{
		DWORD errID = GetLastError();
		wxString err;
		err.Printf("Cannot terminate executable error: %d", errID);
		wxMessageBox(err, "Result", wxOK);
		return;
	}
	bool res = TerminateProcess(hProcess, 0);
	if (!res) {
		DWORD errID = GetLastError();
		wxString err;
		err.Printf("Cannot terminate executable error: %d", errID);
		wxMessageBox(err, "Result", wxOK);
		return;
	}
	ClearLog();
	DWORD te;
	while (!CmdPipeMutex.try_lock());
	te = CloseHandle(hPipeCmd);
	hPipeCmd = nullptr;
	CmdPipeMutex.unlock();
	ZeroMemory(&SubProcInfo, sizeof(SubProcInfo));
	STARTUPINFO sif;
	ZeroMemory(&sif, sizeof(STARTUPINFO));
	res = CreateProcessW(kill_me.path.c_str().AsWChar(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sif, &SubProcInfo);
	Sleep(100);
	if (!res) {
		DWORD errID = GetLastError();
		wxString err;
		err.Printf("Cannot start executable error: %d", errID);
		wxMessageBox(err, "Result", wxOK);
		return;
	}
	kill_me.pid = SubProcInfo.dwProcessId;
	cMain::GetInstance()->SetSelected(&kill_me);
	cMain::GetInstance()->InjectOutside();
		while (!hPipeCmd || hPipeCmd == INVALID_HANDLE_VALUE) {
		hPipeCmd = CreateFile(TEXT("\\\\.\\pipe\\cmd_pipe"),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
	}
	char buffer[1024];
	sprintf(buffer, "Pipe %X control connected!!!\0", hPipeCmd);
	LogMessages->push_back({ (DWORD)LogMessages->size(), false, false, buffer });
	SetAction(COMMANDS::Cemul);
	while (!hPipeEml || hPipeEml == INVALID_HANDLE_VALUE) {
		hPipeEml = CreateFile(TEXT("\\\\.\\pipe\\emul_pipe"),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
	}
	sprintf(buffer, "Pipe %X emulate connected!!!\0", hPipeEml);
	LogMessages->push_back({ (DWORD)LogMessages->size(), false, false, buffer });
	while (!SetLock());
	for (auto it = ListenedMessages->begin(); it != ListenedMessages->end(); it++) {
		std::list<std::string> list = Create_Message((it)->message.data(), (it)->message.size());
		ToEmlPipe(&list);
	}
	UnLock();
	
}

cPipe* cPipe::GetInstance()
{
	if (cpipe == nullptr) {
		cpipe = new cPipe;
	}
	return cpipe;
}

cPipe* cPipe::cpipe = nullptr;

bool cPipe::SetLock()
{
	return DataMutex.try_lock();
}

void cPipe::UnLock()
{
	DataMutex.unlock();
}