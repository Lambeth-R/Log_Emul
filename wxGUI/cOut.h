#ifndef COUT_H
#define COUT_H
#pragma once
#include <vector>
#include <list>
#include "../common/common.h"


// output: dat file (all sizes in hex)
// signature DAT div -> (global). or (local)..
// 1 -> N of items
// for each N ->
//		1) N of fields
//		2) Func name size . Func name
//		3) N of field
//		4) field itself
// result like DAT..1..8.ReadFile.5.3.00f.1d.4a/.../0b. etc...

class cOut
{
public:
	cOut(std::string name);
	cOut(std::list<msg> lData, std::string name = "");
	~cOut();
	bool print();
	std::list<msg>* GetData();
private:
	std::list<msg>* readedData;
	bool correctInit = false;
	DWORD readwrite;
	std::string IntToHex(long long value);
	long long HexToInt(std::string value);
	std::string sName;
	std::vector<std::string> data;
};

#endif // !COUT_H
