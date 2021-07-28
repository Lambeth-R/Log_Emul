#include "sysfunk.h"

cReadFile::cReadFile(std::string bs)
{
	this->base_string = new std::string(bs);
	if (parse())
		correct_inited = true;

}

cReadFile::~cReadFile()
{
}

int str_to_int(std::string val)
{
	int result = 0;
	for (auto it = val.begin(); it != val.end(); it++)
	{
		result += *it._Ptr - 48;
		result *= 10;
	}
	return result / 10;

}

bool cReadFile::parse()
{
	int position = 0;	
	if (base_string->find("ReadFile") != 0 || base_string->back() != ')')
		return false;
	position = 9;
	std::string tstring = base_string->substr(position, base_string->size() - 1);
	this->hFile = tstring.substr(0, tstring.find("|||"));
	tstring = tstring.substr(3 + this->hFile.size(), tstring.size());
	auto it = base_string->rbegin();
	do
	{		
		it++;
		if (*it == '|') break;
		this->lpOverlapped.push_back(*it);		
	} while (!(*(it) == '|' && *(it + 1) == '|' && *(it + 2) == '|'));
	std::reverse(std::begin(lpOverlapped), std::end(lpOverlapped));
	it += 2;
	std::string size_string;
	do
	{
		it++;
		if (*it == '|') break;
		size_string.push_back(*it);		
	} while (!(*(it) == '|' && *(it + 1) == '|' && *(it + 2) == '|'));
	it += 2;
	std::reverse(std::begin(size_string), std::end(size_string));
	this->lpNumberOfBytesRead = str_to_int(size_string);
	size_string.clear();
	do
	{
		it++;
		if (*it == '|') break;
		size_string.push_back(*it);		
	} while (!(*(it) == '|' && *(it + 1) == '|' && *(it + 2) == '|'));
	std::reverse(std::begin(size_string), std::end(size_string));
	this->nNumberOfBytesToRead = str_to_int(size_string);
	this->lpBuffer = tstring.substr(0, this->lpNumberOfBytesRead);
	return true;
}