#include <sstream>
#include <codecvt>
#include "Pipe.h"

Pipe::Pipe(std::wstring name, DWORD type)
{
	exit_code = -1;
	this->name = name;
	this->type = type;
	LogMessages = new std::list<msg>;
	Messages = new std::list<msg>;
	HMODULE kerHandle = GetModuleHandleA("Kernel32.dll");
	if (!kerHandle)
		error = GetLastError();
	else {
		error = 0;
		lpWriteFile = (True_WriteFile)GetProcAddress(kerHandle, "WriteFile");
		lpReadFile = (True_ReadFile)GetProcAddress(kerHandle, "ReadFile");
		if (!(lpReadFile && lpWriteFile))
			error = GetLastError();
		else
			hThread = new std::future<void>(std::async(std::launch::async, &Pipe::WorkThread, this));
	}
}

Pipe::~Pipe()
{
	exit_code = 0;
}

std::list<msg> Pipe::GetLogMessages()
{
	while (!logMutex.try_lock());
	std::list<msg> TempResult = *LogMessages;
	logMutex.unlock();
	return TempResult;
}

void Pipe::PutLogMessages(std::list<msg> mList)
{
	while (!logMutex.try_lock());
	for (auto it = mList.begin(); it != mList.end(); it++) {
		this->LogMessages->push_back(*it);
	}
	logMutex.unlock();
}

std::list<msg> Pipe::GetMessages()
{
	while (!dataMutex.try_lock());
	std::list<msg> TempResult = *Messages;
	dataMutex.unlock();
	return TempResult;
}

void Pipe::PutMessages(std::list<msg> mList)
{
	while (!dataMutex.try_lock());
	for (auto it = mList.begin(); it != mList.end(); it++) {
		this->Messages->push_back(*it);
	}
	dataMutex.unlock();
}

void Pipe::WorkThread()
{
	char buffer[1024];
	DWORD dwRead;
	DWORD err = 0;
	std::wstring pName = L"\\\\.\\pipe\\";
	pName.append(this->name.c_str());
	while (!(this->type > 0));
	switch (this->type)
	{
	case (PIPE_SEND | PIPE_CREATE): {
		Create(pName);
		Send();
		break;
	}
	case (PIPE_RECIEVE | PIPE_CONNECT): {
		Connect(pName);
		Recieve();
		break;
	}
	case (PIPE_RECIEVE | PIPE_CREATE): {
		Create(pName);
		Recieve();
		break;
	}
	case (PIPE_SEND | PIPE_CONNECT): {
		Connect(pName);
		Send();
		break;
	}
	default:
		break;
	}
	
}

bool Pipe::PushToPipe(std::string mToSend) {
	DWORD dwWritten;
	if (mToSend.size() == 0) return false;
	return lpWriteFile(hPipe, mToSend.c_str(), SINGLE_MSG_SIZE, &dwWritten, NULL);
}

bool Pipe::Log_parse(char* buffer, int readed, int* size)
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

bool Pipe::Cmd_parse(char* buffer, int readed)
{
	if (buffer[0] != '2' || buffer[1] != '2') return false;
	if (buffer[2] != 'c' || buffer[3] != 'm' || buffer[4] != 'd') return false;
	if (buffer[5] != '>' || buffer[10] != '<') return false;
	if (buffer[11] == 'l' || buffer[12] == 'o' || buffer[13] == 'g') return true;
	if (buffer[11] == 'e' || buffer[12] == 'm' || buffer[13] == 'l') return true;
	return false;
}

void Pipe::Create(std::wstring pName) {
	hPipe = CreateNamedPipe(pName.c_str(),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
}

void Pipe::Connect(std::wstring pName) {
	while (!hPipe || hPipe == INVALID_HANDLE_VALUE) {
		hPipe = CreateFile(pName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
	}
	while (!this->logMutex.try_lock());
	char buffer[1024];
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::string converted_str = converter.to_bytes(pName);
	sprintf(buffer, "Pipe %X %s connected!!!\0", hPipe, converted_str.c_str());
	LogMessages->push_back({ (DWORD)LogMessages->size(), false, false, std::string(buffer) });
	this->logMutex.unlock();
}

void Pipe::Send() {
	int size = 0;
	while (exit_code < 0)
	{
		Sleep(10);
		while (Messages->size() > size)
		{
			while (!dataMutex.try_lock());
			auto it = Messages->begin();
			std::advance(it, size);
			for (it; it != Messages->end(); it++) {
				PushToPipe(it->message);
				size++;
			}
			dataMutex.unlock();
		}
	}
}

void Pipe::Recieve() {
	char buffer[1024];
	DWORD dwRead;
	while (exit_code < 0)
	{
		std::string message;
		bool end_sequence = false;
		int m_size = 0;
		do {
			while (!pipeMutex.try_lock());
			bool readres = false;
			while (exit_code < 0 && !readres) {
				readres = lpReadFile(hPipe, buffer, SINGLE_MSG_SIZE, &dwRead, NULL);
			}
			pipeMutex.unlock();
			if (exit_code >= 0)
				goto end;
			if (!Log_parse(buffer, dwRead, &m_size)) {
				if(Cmd_parse(buffer, dwRead)){
					message.append(std::string(&buffer[11], 3));
					end_sequence = true;
					break;
				}
				continue;
			}
			if (std::string(&buffer[11]).find("end@#$?") == 0)
				end_sequence = true;
			else
				message.append(std::string(&buffer[11], m_size));
		} while (!end_sequence);
		Messages->push_back({ (DWORD)Messages->size(), false, false, message });
		if (exit_code >= 0)
			goto end;
	}
end:
	CloseHandle(hPipe);
	hPipe = nullptr;
}

void Pipe::AddSingleMessage(std::string message)
{
	Messages->push_back(msg{(DWORD) Messages->size(), false, false, message });
}

void Pipe::ClearLog()
{
	LogMessages->clear();
}