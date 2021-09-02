#include <string>
#include <sstream>
#include <list>
#include "../common/common.h"


int main()
{
	std::list<msg>* lMsgs = ReadFromLog("D:\\Docs\\Диплом\\wxGUI\\wxGUI\\Mon_Aug_23_00_19_12_2021.dat");
	std::string test = CreateLogText(*lMsgs);
	return 0;
}