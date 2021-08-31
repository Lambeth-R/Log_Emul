#include <string>
#include <sstream>
#include <list>
#include <Windows.h>

//Working (but worth it?)

void test(std::list<std::stringstream*> FunkStack)
{
	int _1;
	std::string _2;
	double _3;
	std::string _4;
	auto it = FunkStack.begin();
	**it >> _1;
	it++;
	**it >> _2;
	it++;
	**it >> _3;
	it++;
	**it >> _4;
	it++;
	return;
}

int main()
{
	int _1 = 0;
	std::string _2 = "TestStr";
	double _3 = 11.04;
	HMODULE kerHandle = GetModuleHandleA("Kernel32.dll");
	std::list<std::stringstream*> *lsStack = new std::list<std::stringstream*>;
	std::stringstream s_1, s_2, s_3, s_4;
	s_1 << _1;
	s_2 << _2;
	s_3 << _3;
	s_4 << kerHandle;
	lsStack->push_back(&s_1);
	lsStack->push_back(&s_2);
	lsStack->push_back(&s_3);
	lsStack->push_back(&s_4);
	test(*lsStack);
	return 0;
}