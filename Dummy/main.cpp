#include <string>
#include "Pipe.h"

int main()
{
	//std::string test("ReadFile(19|||Engish, moetherfucker, do you speak it?|||39|||39|||NULL)");
	//class foo : public Multiton<foo, DWORD> {};
	//foo* f = foo::getPtr(1);
	Pipe* p = Pipe::GetInstance(L"log_pipe", PIPE_CREATE | PIPE_LISTEN);
	Pipe* p1 = Pipe::GetInstance(L"log_pipe", PIPE_CREATE | PIPE_LISTEN);
	//Pipe p1(L"log_pipe", PIPE_CREATE | PIPE_LISTEN);
	//Pipe p2(L"cmd_pipe", PIPE_CREATE | PIPE_LISTEN);
	//Pipe p3(L"eml_pipe", PIPE_CREATE | PIPE_EMULATE);
	Sleep(100000);
	return 0;
}