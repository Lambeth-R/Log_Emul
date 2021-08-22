#include "cOut.h"
#include <sstream>
#include <chrono>
#include <ctime> 
#include <fstream>
#include <iostream>
#include "sysfunk.h"



cOut::cOut(std::string name)
{
	readwrite = 1;
	std::string wholeFile;
	std::ifstream file(name, std::ios::in | std::ios::binary);
	if (!file)
		return;
	file.seekg(0, file.end);
	std::streamsize size = file.tellg();
	char* tbuffer = new char[size];
	file.seekg(0, std::ios::beg);
	file.read(tbuffer, size);
	wholeFile.append(tbuffer, size);
	delete[] tbuffer;
	if (wholeFile.find("DAT..") != 0)
		return;
	wholeFile = wholeFile.substr(5);
	if (wholeFile.find("..") == -1)
		return;
	readedData = new std::list<msg>;
	DWORD dItems = HexToInt(wholeFile.substr(0, wholeFile.find("..")));
	wholeFile = wholeFile.substr(wholeFile.find("..") + 2);
	for (DWORD i = 0; i < dItems; i++) {
		std::string recoveredInfo;
		DWORD szfName = HexToInt(wholeFile.substr(0, wholeFile.find(".")));
		wholeFile = wholeFile.substr(wholeFile.find(".") + 1);
		recoveredInfo.append(wholeFile.substr(0,szfName));
		DWORD dFields = 0;
		switch (enumFucs.find(recoveredInfo)->second)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			dFields = 5;
			break;
		case 3:
			break;
		default:
			break;
		}
		recoveredInfo.append("(");
		wholeFile = wholeFile.substr(szfName);
		for (DWORD j = 0; j < dFields; j++) {
			if (wholeFile.find('.') != 0)
				return;
			wholeFile = wholeFile.substr(1);
			if (wholeFile.find('.') == -1)
				return;
			DWORD szNext = HexToInt(wholeFile.substr(0, wholeFile.find('.')));
			wholeFile = wholeFile.substr(wholeFile.find('.') + 1);
			recoveredInfo.append(wholeFile.substr(0, szNext));
			recoveredInfo.append("|||");
			wholeFile = wholeFile.substr(szNext);
		}
		recoveredInfo = recoveredInfo.substr(0, recoveredInfo.size() - 3);
		recoveredInfo.append(")");
		if (wholeFile.size() >= 2) {
			if (wholeFile.find('..') != -1)
				wholeFile = wholeFile.substr(wholeFile.find('..') + 2);
			else
				break;
		}
		readedData->push_back({ (DWORD) readedData->size(), false, false, recoveredInfo });
	}
	correctInit = true;
}

cOut::cOut(std::list<msg> lData, std::string name)
{
	readwrite = 2;
	for (auto it = lData.begin(); it != lData.end(); it++) {
		std::string result, fName, strRst;
		fName = it->message.substr(0, it->message.find("("));
		//Func Name size
		result.append(IntToHex(fName.size()));
		result.append(".");
		//Func Name
		result.append(fName);
		result.append(".");
		auto t = enumFucs.find(fName);
		strRst = it->message.substr(it->message.find("(") + 1);
		DWORD fields = 0;
		switch (t->second)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			fields = 5;
			break;
		case 3:
			break;
		default:
			break;
		}
		for (DWORD i = 0; i < fields; i++) {
			if (strRst.find("|||") != -1) {
				std::string temp = strRst.substr(0, strRst.find("|||"));
				result.append(IntToHex(temp.size()));
				result.append(".");
				result.append(temp);
				result.append(".");
				strRst = strRst.substr(temp.size() + 3);
			}
			else {
				result.append(IntToHex(strRst.size() - 1));
				result.append(".");
				result.append(strRst.substr(0, strRst.size() - 1));
				result.append("..");
			}

		}
		data.push_back(result);
	}
	if (name.size() == 0) {
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		sName = std::ctime(&time);
		sName = sName.substr(0,sName.size() - 1);
	}
	else sName = name;
	for (auto it = sName.begin(); it != sName.end(); it++) {
		if (*it == ':' || *it == '*' || *it == '\\' || *it == '\/' || *it == '\|' || *it == '\"' || *it == '?' || *it == '<' || *it == '>' || *it == ' ')
			*it = '_';
	}
	sName.append(".dat");
	correctInit = true;
}

cOut::~cOut()
{
	if (correctInit && readwrite == 1)
	{
		delete readedData;
	}
}

bool cOut::print()
{
	if (readwrite != 2 || !correctInit)
		return false;
	std::filebuf file;
	file.open(sName, std::ios::out | std::ios::binary);
	if (!file.is_open())
		return false;
	std::ostream os(&file);
	os << "DAT..";
	os << IntToHex(data.size());
	os << "..";
	for (auto it = data.begin(); it != data.end(); it++) {
		os << *it;
	}
	file.close();
	return true;
}

std::list<msg>* cOut::GetData()
{
	if (readwrite != 1 || !correctInit)
		return nullptr;
	return readedData;
}

std::string cOut::IntToHex(long long value)
{
	std::stringstream ss;
	ss << std::hex << value;
	return ss.str();
}

long long cOut::HexToInt(std::string value)
{
	long long result;
	std::stringstream ss;
	ss << std::hex << value;
	ss >> result;
	return result;
}