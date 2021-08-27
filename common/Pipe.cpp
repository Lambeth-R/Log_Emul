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
	//Here was useless init of ReadFile / Writefile from kernel32. Useless cos that pointers is exactly what was changed. Now need to use SetRightFuncs
	hThread = new std::future<void>(std::async(std::launch::async, &Pipe::WorkThread, this));
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
	std::unique_lock<std::mutex> ul(muxWait);
	cvBlock.notify_one();
}

void Pipe::WorkThread()
{
	if (!correct_init) {
		std::unique_lock<std::mutex> ul(muxInit);
		cvInit.wait(ul);
	}
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
	while (!pipeMutex.try_lock());
	bool res = lpWriteFile(hPipe, mToSend.c_str(), SINGLE_MSG_SIZE, &dwWritten, NULL);
	pipeMutex.unlock();
	return res;
}

bool Pipe::parsable(char* buffer, int readed, int* size)
{
	std::string temp = buffer;
	temp = temp.substr(0, 11);
	if (temp.find("22") != 0)
		return false;
	if (temp.find(keyword) != 2)
		return false;
	if (buffer[5] != '>' || buffer[10] != '<')
		return false;
	buffer[10] = 0;
	std::stringstream stream;
	stream << &buffer[6];
	stream >> std::hex >> *size;
	return true;
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

std::string Pipe::wtochar(std::wstring string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::string res = converter.to_bytes(string);
	return res;
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
	sprintf(buffer, "Pipe %X %s connected!!!\0", hPipe, wtochar(pName).c_str());
	LogMessages->push_back({ (DWORD)LogMessages->size(), false, false, std::string(buffer) });
	this->logMutex.unlock();
}

void Pipe::wait(int size)
{
	while (!(Messages->size() > size))
	{
		std::unique_lock<std::mutex> ul(muxWait);
		cvBlock.wait(ul);
	}
}

void Pipe::Send() {
	int size = 0;
	while (exit_code < 0)
	{
		Sleep(10);
		wait(size);
		while (Messages->size() > size)
		{
			while (!dataMutex.try_lock());
			auto it = Messages->begin();
			std::advance(it, size);
			std::string init_message = "22ini>", message = wtochar(name.substr(0, 3));
			std::stringstream stream1, stream2;
			int mcount = ceil((double)it->message.size() / SINGLE_MSG_SIZE);
			stream1 << std::hex << mcount;
			message.append(stream1.str());
			stream2 << std::hex << (int)message.size();
			char _size[4];
			memset(_size, 48, 4 * sizeof(char));
			int div = 0;
			if (message.size() < 16) div = 3;
			else if (message.size() < 256) div = 2;
			else if (message.size() < 4096) div = 1;
			else if (message.size() < 65535) div = 0;
			memcpy(&_size[div], stream2.str().c_str(), sizeof(char) * (4 - div));
			init_message.append(_size,4);
			init_message.append("<");
			init_message.append(message);
			PushToPipe(init_message);
			std::string sending = it->message;
			do {
				std::string single_msg = "22";
				single_msg.append(wtochar(name.substr(0, 3)));
				single_msg.append(">");
				int div = sending.size() > SINGLE_MSG_SIZE ? SINGLE_MSG_SIZE - single_msg.length() - 6 : sending.size();
				std::stringstream stream3;
				stream3 << std::hex << div;
				char _size[4];
				memset(_size, 48, 4 * sizeof(char));
				int off = 0;
				if (div < 16) off = 3;
				else if (div < 256) off = 2;
				else if (div < 4096) off = 1;
				memcpy(&_size[off], stream3.str().c_str(), sizeof(char) * (4 - off));
				single_msg.append(_size, 4);
				single_msg.append("<");
				single_msg.append(sending.substr(0, div));
				sending = sending.substr(div);
				PushToPipe(single_msg);
			} while (sending.size() > 0);
			size++;
			dataMutex.unlock();
		}
	}
}

void Pipe::Recieve() {
	char buffer[1024];
	DWORD dwRead;

	while (exit_code < 0)
	{
		int m_size = 0;
		//Init message
		while (!pipeMutex.try_lock());
		bool readres = false;
		while (exit_code < 0 && !readres) {
			readres = lpReadFile(hPipe, buffer, SINGLE_MSG_SIZE, &dwRead, NULL);
		}
		pipeMutex.unlock();
		std::string init_message = buffer;
		if (init_message.find("22ini") != 0)
			continue;
		keyword = init_message.substr(init_message.find("<") + 1, 3);
		std::stringstream stream1, stream2;
		int _size = 0;
		stream1 << std::hex << init_message.substr(init_message.find(">") + 1, 4);
		stream1 >> _size;
		_size -= 3;
		stream2 << std::hex << init_message.substr(init_message.find("<") + 4, _size);
		stream2 >> _size;
		int i = 0;
		std::string message;
		//char* message = new char[_size * SINGLE_MSG_SIZE];
		do {
			while (!pipeMutex.try_lock());
			readres = false;
			while (exit_code < 0 && !readres) {
				readres = lpReadFile(hPipe, buffer, SINGLE_MSG_SIZE, &dwRead, NULL);
			}
			pipeMutex.unlock();
			if (exit_code >= 0)
				goto end;
			if (!parsable(buffer, dwRead, &m_size)) {
				continue;
			}
			//memcpy(&message[i * SINGLE_MSG_SIZE], &buffer[11], m_size);
			//if (m_size < SINGLE_MSG_SIZE)
			//	message[m_size] = '\0';
			message.append(std::string(&buffer[11], m_size));
			i++;
		} while (i < _size);
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
	while (!dataMutex.try_lock());
	Messages->push_back(msg{ (DWORD)Messages->size(), false, false, message });
	dataMutex.unlock();
	std::unique_lock<std::mutex> ul(muxWait);
	cvBlock.notify_one();
}

void Pipe::ClearLog()
{
	LogMessages->clear();
}

void Pipe::SetRightFuncs(True_WriteFile TrueWriteFile, True_ReadFile TrueReadFile)
{
	lpWriteFile = TrueWriteFile;
	lpReadFile = TrueReadFile;
	correct_init = true;
	std::unique_lock<std::mutex> ul(muxInit);
	cvInit.notify_one();
}