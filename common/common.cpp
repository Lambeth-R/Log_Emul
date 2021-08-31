#include <list>
#include <sstream>
#include "common.h"
#include "Pipe.h"

std::wstring pipename[] = { L"cmd_pipe",L"log_pipe",L"eml_pipe" };

std::list<msg>* Div_Messages(char* text, long long length, const char pref[])
{
	std::list<msg>* ResList = new std::list<msg>;
	do
	{
		std::string single_msg;
		single_msg.append("22");
		single_msg.append(pref);
		single_msg.append(">");
		std::stringstream stream1;
		int div = length > SINGLE_MSG_SIZE ? SINGLE_MSG_SIZE - single_msg.length() - 6 : length;
		stream1 << std::hex << div;
		char _size[4];
		memset(_size, 48, 4 * sizeof(char));
		int off = 0;
		if (div < 16) off = 3;
		else if (div < 256) off = 2;
		else if (div < 4096) off = 1;
		memcpy(&_size[off], stream1.str().c_str(), sizeof(char) * (4 - off));
		single_msg.append(_size, 4);
		single_msg.append("<");
		single_msg.append(text, div);
		memset(text, 0, sizeof(char) * div);
		memcpy(text, &text[div], sizeof(char) * (length - div));
		length -= div;
		ResList->push_back({ (DWORD)ResList->size(),false,false,single_msg });
	} while (length > 0);
	return ResList;
}

std::string wtochar(std::wstring string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::string res = converter.to_bytes(string);
	return res;
}