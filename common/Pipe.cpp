#include <sstream>
#include <codecvt>
#include "Pipe.h"

// TODO Close pipe messages

Pipe::Pipe(std::wstring name, DWORD type)
{
	exit_code = -1;
	this->name = name;
	this->type = type;
	LogMessages = new std::list<msg>;
	Messages = new std::list<msg>;
	muxExtern = new std::mutex;
	cvExtern = new std::condition_variable;
	//Here was useless init of ReadFile / Writefile from kernel32. Useless cos that pointers is exactly what was changed. Now need to use SetRightFuncs
	hThread = new std::future<void>(std::async(std::launch::async, &Pipe::WorkThread, this));
}

Pipe::~Pipe()
{
	if (LogMessages)
		delete LogMessages;
	if (Messages)
		delete Messages;
	if (muxExtern)
		delete muxExtern;
	if (cvExtern)
		delete cvExtern;
	exit_code = 0;
}

void Pipe::SetExitCode(int code) {
	exit_code = code;
}

std::list<msg> Pipe::GetLogMessages()
{
	while (!muxLog.try_lock());
	std::list<msg> TempResult = *LogMessages;
	muxLog.unlock();
	return TempResult;
}

std::list<msg> Pipe::GetMessages()
{
	while (!muxData.try_lock());
	std::list<msg> TempResult = *Messages;
	muxData.unlock();
	return TempResult;
}

void Pipe::PutLogMessages(std::list<msg> mList)
{
	while (!muxLog.try_lock());
	for (auto it = mList.begin(); it != mList.end(); it++) {
		PutSingleLogMessage(it->message);
	}
	muxLog.unlock();
}

void Pipe::PutSingleLogMessage(std::string sMsg)
{
	LogMessages->push_back({ (DWORD)LogMessages->size(), false, false, std::string(sMsg) });
	std::unique_lock<std::mutex> ul1(muxWait);
	cvBlock.notify_one();
	std::unique_lock<std::mutex> ul2(*muxExtern);
	cvExtern->notify_one();
}

void Pipe::PutMessages(std::list<msg> mList)
{
	while (!muxData.try_lock());
	for (auto it = mList.begin(); it != mList.end(); it++) {
		PutSingleMessage(it->message);
	}
	muxData.unlock();
}

void Pipe::PutSingleMessage(std::string sMsg)
{
	Messages->push_back( {(DWORD) Messages->size(), false, false, sMsg});
	std::unique_lock<std::mutex> ul1(muxWait);
	cvBlock.notify_one();
	std::unique_lock<std::mutex> ul2(*muxExtern);
	cvExtern->notify_one();
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
	while (!muxPipe.try_lock());
	bool res = lpWriteFile(hPipe, mToSend.c_str(), SINGLE_MSG_SIZE, &dwWritten, NULL);
	muxPipe.unlock();
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

void Pipe::Connect(std::wstring pName) {
	while (!hPipe || hPipe == INVALID_HANDLE_VALUE) {
		hPipe = CreateFile(pName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
	}
	while (!muxLog.try_lock());
	char buffer[1024];
	sprintf(buffer, "Pipe %X %s connected!!!\0", hPipe, wtochar(pName).c_str());
	PutSingleLogMessage(buffer);
	muxLog.unlock();
}

void Pipe::wait(int size)
{
	while (!(Messages->size() > size))
	{
		std::unique_lock<std::mutex> ul(muxWait);
		cvBlock.wait(ul);
	}
}

int calc_div(std::string message)
{
	if (message.size() < 16) return 3;
	else if (message.size() < 256) return 2;
	else if (message.size() < 4096) return 1;
	else if (message.size() < 65535) return 0;
}

void Pipe::Send() {
	int size = 0;
	while (exit_code < 0)
	{
		Sleep(10);
		wait(size);
		while (Messages->size() > size)
		{
			while (!muxData.try_lock());
			auto it = Messages->begin();
			std::advance(it, size);
			std::string init_message = "22ini>", message = wtochar(name.substr(0, 3));
			std::stringstream stream1, stream2;
			int mcount = ceil((double)it->message.size() / (SINGLE_MSG_SIZE - HEADER_SIZE));
			stream1 << std::hex << mcount;
			message.append(stream1.str());
			stream2 << std::hex << (int)message.size();
			char _size[4];
			memset(_size, 48, 4 * sizeof(char));
			int div = calc_div(message);
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
				div = sending.size() > (SINGLE_MSG_SIZE - HEADER_SIZE) ? SINGLE_MSG_SIZE - HEADER_SIZE : sending.size();
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
			muxData.unlock();
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
		while (!muxPipe.try_lock());
		bool readres = false;
		while (exit_code < 0 && !readres) {
			readres = lpReadFile(hPipe, buffer, SINGLE_MSG_SIZE, &dwRead, NULL);
		}
		muxPipe.unlock();
		if (exit_code > 0)
			return;
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
		do {

			while (!muxPipe.try_lock());
			readres = false;
			while (exit_code < 0 && !readres) {
				readres = lpReadFile(hPipe, buffer, SINGLE_MSG_SIZE, &dwRead, NULL);
			}
			muxPipe.unlock();

			if (exit_code >= 0)
				goto end;

			if (!parsable(buffer, dwRead, &m_size)) {
				continue;
			}
			message.append(std::string(&buffer[11], m_size));
			i++;
		} while (i < _size);
		PutSingleMessage(message);
		if (exit_code >= 0)
			goto end;
	}
end:
	CloseHandle(hPipe);
	hPipe = nullptr;
}

void Pipe::ClearLog()
{
	LogMessages->clear();
}

void Pipe::SetRightFuncs(True_WriteFile TrueWriteFile, True_ReadFile TrueReadFile)
{
	if (correct_init == true)
		return;
	lpWriteFile = TrueWriteFile;
	lpReadFile = TrueReadFile;
	correct_init = true;
	std::unique_lock<std::mutex> ul(muxInit);
	cvInit.notify_one();
}