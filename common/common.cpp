#include <list>
#include <sstream>
#include "common.h"
#include "Pipe.h"
#include <fstream>

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

long long HexToInt(std::string value)
{
	long long result;
	std::stringstream ss;
	ss << std::hex << value;
	ss >> result;
	return result;
}

std::string IntToHex(long long value)
{
	std::stringstream ss;
	ss << std::hex << value;
	return ss.str();
}

// Convert stringstream to string
std::string Custom_Create(std::list<std::stringstream*> FuncStack)
{
	std::string res;
	//switch (FunkId)
	//{
	//case 1:
	//{
	//	res.append("ReadFile(");
	//	break;
	//}
	//case 2:
	//{
	//	res.append("WriteFile(");
	//	break;
	//}
	//default:
	//	break;
	//}
	res.append(IntToHex(FuncStack.size()));
	res.append(".");
	for (auto it : FuncStack) {
		res.append(IntToHex(it->str().size()));
		res.append(".");
		res.append(it->str());
		res.append(".");
	}
	res.append(".");
	return res;
}

bool ParseSingle(std::string orig_message, std::list<std::stringstream*>* result, int* offset)
{
	long long div = orig_message.find(".");
	int ItemCount = HexToInt(orig_message.substr(0, div));	
	orig_message = orig_message.substr(div + 1);
	*offset += div + 1;
	for (int i = 0; i < ItemCount; i++) {
		div = orig_message.find(".");
		int FieldSize = HexToInt(orig_message.substr(0, div));		
		orig_message = orig_message.substr(div + 1);
		*offset += div + 1;
		std::stringstream* field = new std::stringstream;
		*field << orig_message.substr(0, FieldSize);		
		orig_message = orig_message.substr(FieldSize);
		*offset += FieldSize;
		if (orig_message[0] != '.')
			return false;
		orig_message = orig_message.substr(1);
		(*offset)++;		
		result->push_back(field);
	}
	return true;
}
// Whole file std::string to msg list
std::list<msg>* ParseFile(std::string wholeFile)
{
	std::list<msg>* readedData = new std::list<msg>;
	if (wholeFile.find("DAT..") == 0) {
		int* offset = new int;		
		wholeFile = wholeFile.substr(5);
		long long div = wholeFile.find(".");
		int ItemCount = HexToInt(wholeFile.substr(0, div));
		wholeFile = wholeFile.substr(div + 2);		
		for (int i = 0; i < ItemCount; i++) {
			*offset = 0;
			std::list<std::stringstream*>* record = new std::list<std::stringstream*>;
			if (ParseSingle(wholeFile, record, offset)) {
				wholeFile = wholeFile.substr(*offset+1);
				readedData->push_back({ (DWORD)readedData->size(), false, false, Custom_Create(*record) });
			}
			else
				return readedData;
		}
	}
	return readedData;
}
// Creates file string to be writed in log file
std::string CreateLogText(std::list<msg> lMessages) {
	std::string result = "DAT..";
	result.append(IntToHex(lMessages.size()));
	result.append("..");
	for (auto it : lMessages) {
		std::list<std::stringstream*>* fields = new std::list<std::stringstream*>;
		std::string* Fname = new std::string;
		int* useless = new int;
		*useless = 0;
		if (ParseSingle(it.message, fields, useless)) {
			result.append(IntToHex(fields->size()));
			result.append(".");
			for (auto itField : *fields) {
				result.append(IntToHex(itField->str().size()));
				result.append(".");
				result.append(itField->str());
				result.append(".");
			}
			result.append(".");
		}
	}
	return result;
}
//Call to read file
std::list<msg>* ReadFromLog(std::string name) {
	std::string* wholeFile = new std::string;
	std::ifstream file(name, std::ios::in | std::ios::binary);
	if (!file)
		return nullptr;
	file.seekg(0, file.end);
	std::streamsize size = file.tellg();
	char* tbuffer = new char[size];
	file.seekg(0, std::ios::beg);
	file.read(tbuffer, size);
	wholeFile->append(tbuffer, size);
	delete[] tbuffer;
	if (wholeFile->find("DAT..") != 0)
		return nullptr;	
	return ParseFile(*wholeFile);
}
//Call to print data to file
bool CreateLogFile(std::list<msg> lMessages, std::string* name) {
	if (name->size() == 0) {
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		*name = std::ctime(&time);
		*name = name->substr(0, name->size() - 1);
	}
	for (auto it = name->begin(); it != name->end(); it++) {
		if (*it == ':' || *it == '*' || *it == '\\' || *it == '\/' || *it == '\|' || *it == '\"' || *it == '?' || *it == '<' || *it == '>' || *it == ' ')
			*it = '_';
	}
	name->append(".dat");
	std::filebuf file;
	file.open(*name, std::ios::out | std::ios::binary);
	if (!file.is_open())
		return false;
	std::ostream os(&file);
	std::string data = CreateLogText(lMessages);
	os << data;
	file.close();

}

std::wstring s2ws(const std::string& str)
{

	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}