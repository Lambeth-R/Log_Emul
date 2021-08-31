#include <future>
#include "Emulater.h"


void Emulater::Activate()
{
	active = true;
}

void Emulater::SwitchContext(std::string message) {
	if (message.find("log") == 0) {
		CurrentState = COMMANDS::Cloggin;
		Pipe* lPipe = Pipe::GetInstance(pipename[1], PIPE_CREATE | PIPE_SEND);
		lPipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
	}
	if (message.find("eml") == 0) {
		CurrentState = COMMANDS::Cemul;
		Pipe* ePipe = Pipe::GetInstance(pipename[2], PIPE_CREATE | PIPE_RECIEVE);
		ePipe->SetRightFuncs(TrueWriteFile, TrueReadFile);
		hThread = new std::future<void>(std::async(std::launch::async, &Emulater::SyncMsg, this));
	}
}

Emulater::Emulater()
{

}
Emulater::~Emulater()
{

}

void Emulater::wait(Pipe* Pipe, int size)
{
	while (!(Pipe->GetMessages().size() > size))
	{
		std::unique_lock<std::mutex> ul(*Pipe->muxExtern);
		Pipe->cvExtern->wait(ul);
	}
}

void Emulater::SyncMsg()
{
	while (CurrentState == COMMANDS::Cemul) {
		Pipe* ePipe = Pipe::GetInstance(pipename[2], PIPE_CREATE | PIPE_RECIEVE);
		wait(ePipe, Messages.size());
		auto temp = ePipe->GetMessages();
		auto it = temp.begin();
		std::advance(it, Messages.size());
		while (it != temp.end()) {
			Messages.push_back(*it);
			it++;
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