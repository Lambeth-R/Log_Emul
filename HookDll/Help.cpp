#define _CRT_SECURE_NO_WARNINGS	
#include "Funks.h"
#include "Help.h"

bool equal_strings(char* st, char* nd, int size) {
	for (int i = 0; i < size; i++)
		if (st[i] != nd[i]) return 0;
	return 1;
}

std::vector<char> PIHAY_V_VECTOR(char* str, int size)
{
	std::vector<char> buff;
	buff.assign(str, str + size);
	return buff;
}

bool parse_messge(STRING rcv, sMessage* result) {
	//if (rcv[0] != result->start_seq[0] || rcv[1] != result->start_seq[1]) return 0;
	memcpy(result->packORD, &rcv[2], 4);
	memcpy(result->CMD, &rcv[6], 3);
	memcpy(result->size, &rcv[9], 3);
	char temp[10];
	memcpy(temp, result->size, 3);
	temp[3] = '\0';
	sscanf(temp, "%x", &result->int_size);
	memcpy(result->char_string, &rcv[12], result->int_size);
	return 1;
}

STRING make_msg(char snd[1012], int sz, const char command[3]) {
	std::vector<char> res, t;
	sMessage* one = new sMessage;
	if (sz == 1024) sz -= 12;
	res = PIHAY_V_VECTOR((char*)one->start_seq, 2);
	if (packID > 9999) packID = 0;

	int temp = packID, i = 3;
	while (temp != 0) {
		one->packORD[i] = temp % 10 + 48;
		i--;
		temp -= temp % 10;
		temp /= 10;
	}
	t = PIHAY_V_VECTOR(one->packORD, 4);
	for (auto j = t.begin(); j < t.end(); j++)
	{
		res.push_back(*j);
	}
	strcpy(one->CMD, command);
	t = PIHAY_V_VECTOR(one->CMD, 3);
	for (auto j = t.begin(); j < t.end(); j++)
	{
		res.push_back(*j);
	}
	char tsize[10];
	sprintf(tsize, "%x", sz);
	memcpy(one->size, tsize, sizeof(char) * 3);
	for (int j = 0; j < 3; j++)
	{
		res.push_back(tsize[j]);
	}
	char* message = new char[1024];
	memset(message, '\0', 1024);
	memcpy(message, snd, sz);
	t = PIHAY_V_VECTOR(message, 1012);
	for (auto j = t.begin(); j < t.end(); j++)
	{
		res.push_back(*j);
	}
	packID++;
	return res;
}