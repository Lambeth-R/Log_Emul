#include <future>
#include "Emulater.h"


void Emulater::Activate()
{
	active = true;
}

void Emulater::CloseOtherSide()
{
	if (lPipe) {
		lPipe->PutSingleMessage("!");
		lPipe->SetExitCode(0);
	}
	if (ePipe) {
		ePipe->PutSingleMessage("!");
		ePipe->SetExitCode(0);
	}
}

void Emulater::SwitchContext(std::string message) {
	if (message.find("log") == 0) {
		CurrentState = COMMANDS::Cloggin;
		lPipe = Pipe::GetInstance(pipename[1], PIPE_CREATE | PIPE_SEND);
		lPipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
	}
	if (message.find("eml") == 0) {
		CurrentState = COMMANDS::Cemul;
		ePipe = Pipe::GetInstance(pipename[2], PIPE_CREATE | PIPE_RECIEVE);
		ePipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
		hThread = new std::future<void>(std::async(std::launch::async, &Emulater::SyncMsg, this));
	}
}


Emulater::Emulater()
{
	std::list<msg>* lm1 = new std::list<msg>;
	std::list<msg>* lm2 = new std::list<msg>;
	std::list<msg>* lm3 = new std::list<msg>;
	std::list<msg>* lm4 = new std::list<msg>;
	messages.insert(My_Map::value_type(std::string("CreateFile"), lm1));
	messages.insert(My_Map::value_type(std::string("ReadFile"), lm2));
	messages.insert(My_Map::value_type(std::string("WriteFile"), lm3));
	messages.insert(My_Map::value_type(std::string("CloseHandle"), lm4));
	dCurrMess = 0;
}
Emulater::~Emulater()
{
	for (auto it : messages)
		delete it.second;
}

void Emulater::wait(Pipe* Pipe, int size)
{
	while (!(Pipe->GetMessages().size() > size))
	{
		std::unique_lock<std::mutex> ul(Pipe->muxExtern);
		Pipe->cvExtern.wait(ul);
	}
}

void Emulater::SyncMsg()
{
	DWORD msg_count = 0;
	while (CurrentState == COMMANDS::Cemul) {
		ePipe = Pipe::GetInstance(pipename[2], PIPE_CREATE | PIPE_RECIEVE);
		wait(ePipe, msg_count);
		auto temp = ePipe->GetMessages();
		auto it = temp.begin();
		std::advance(it, msg_count);
		while (it != temp.end()) {
			std::string msg = it->message;
			msg = msg.substr(msg.find(".")+1);
			int fnsize = HexToInt(msg.substr(0, msg.find('.')));
			msg = msg.substr(msg.find(".") + 1);
			std::string fname = msg.substr(0, fnsize);
			while (!mux_data.try_lock());
			auto item = messages.find(fname);
			item->second->push_back(*it);
			msg_count++;
			it++;
			mux_data.unlock();
		}

	}
}

Emulater* Emulater::GetInstance()
{
	if (emulater == nullptr) {
		emulater = new Emulater;
	}
	return emulater;
}

Emulater* Emulater::emulater = nullptr;