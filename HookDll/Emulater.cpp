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
	}
}

Emulater::Emulater()
{

}
Emulater::~Emulater()
{

}

Emulater* Emulater::GetInstance()
{
	if (emulater == nullptr) {
		emulater = new Emulater;
	}
	return emulater;
}

Emulater* Emulater::emulater = nullptr;